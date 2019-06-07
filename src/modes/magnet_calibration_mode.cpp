#include <platform/communication_control.h>
#include <platform/led_control.h>
#include <platform/magnet_control.h>
#include <platform/adc_control.h>
#include <platform/platform_control.h>
#include "magnet_calibration_mode.h"
#include "print.h"

void TMagnetCalibrationMode::switchOn()
{
    TLedControl::enableSlave();
    TLedControl::set(4, 1);
    TLedControl::enableOutput();
    TAdcControl::enableCalibration();
    PulseWidth = Pulses+5;
    onTimer();
    TMagnetControl::start();
}

void TMagnetCalibrationMode::switchOff()
{
    TMagnetControl::stop();
    TLedControl::stop();
    TAdcControl::disableCalibration();
    TLedControl::disableGated();
}

void TMagnetCalibrationMode::onDma()
{
    TLedControl::stop();
    if (PulseWidth == 0) {
        return;
    }
    int v = 0;
    if (PulseWidth==Pulses) {
        for (int i = Samples-4; i<=Samples+4; ++i) {
            v += RawData[i];
        }
        TargetValue = v/9;
        MeasureDelays[Pulses-1] = Samples;
    } else if (PulseWidth < Pulses){
        for (int i = 0; i<Samples+5; i++) {
            if (i<9) {
                v += RawData[i];
            } else if (v<TargetValue*9) {
                MeasureDelays[PulseWidth-1] = i - 5;
                return;
            } else {
                v = v+RawData[i]-RawData[i-9];
            }
        }
        MeasureDelays[PulseWidth-1] = Samples;
    }
}

void TMagnetCalibrationMode::onTimer()
{
    if (PulseWidth == 1) {
        PulseWidth = 0;
        TMagnetControl::stop();
    } else {
        PulseWidth--;
        TMagnetControl::set(13000, PulseWidth * 20, 0);
        TAdcControl::set(RawData, Samples + 5);
    }
}

static const char targetStr[] = "static const int MeasureTarget = ";
static const char endTarget[] = ";\n";
static const char waitStr[] = "static constexpr int MeasureTime[] = { ";
static const char timesStr[] = "static constexpr int PulseTime[] = { ";
static const char sepStr[] = ", ";
static const char endStr[] = " };\n";

inline int round(double num)
{
    return (static_cast<int>(num*2)+1)/2;
}

void TMagnetCalibrationMode::calculate(TStream& stream)
{
    // suppress hard fails
    double tmp[Pulses];
    // smooth curve
    double acc = 0;
    for (int i = 0; i<Pulses; ++i) {
        if (i<9) {
            acc += MeasureDelays[i];
            if (i%2==0) {
                tmp[i/2] = acc/(i+1.0);
            }
        } else {
            acc = acc+MeasureDelays[i]-MeasureDelays[i-9];
            tmp[i-4] = acc/9.0;
        }
    }

    // linearize curve
    int base = 0;
    int values[50] = {0};
    int times[50] = {0};
    int found = 1;
    while (base+2<Pulses) {
        double baseValue = tmp[base];
        bool changed = false;
        for (int test = base+2; test<Pulses; ++test) {
            double testValue = tmp[test];
            bool passed = true;
            for (int i = base+1; i<test; ++i) {
                double predicted = baseValue+(testValue-baseValue)*(i-base)/(test-base);
                if (predicted-tmp[i]>1 || tmp[i]-predicted>1) {
                    passed = false;
                    break;
                }
            }
            if (!passed) {
                changed = true;
                values[found] = round(tmp[base] * 5);
                times[found] = (base + 1) * 20;
                found++;
                base = test-1;
                break;
            }
        }
        if (!changed) {
            break;
        }
    }
    values[found] = round(tmp[Pulses-5] * 5);
    times[found] = Pulses * 20;
    found++;

    stream.print(targetStr);
    stream.print(TargetValue);
    stream.print(endTarget);
    stream.print(waitStr);
    for (int i = 0; i < found; ++i) {
        if (i != 0) stream.print(sepStr);
        stream.print(values[i]);
    }
    stream.print(endStr);
    stream.print(timesStr);
    for (int i = 0; i < found; ++i) {
        if (i != 0) stream.print(sepStr);
        stream.print(times[i]);
    }
    stream.print(endStr);
}

