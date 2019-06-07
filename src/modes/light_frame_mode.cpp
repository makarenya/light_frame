#include <platform/platform_control.h>
#include "light_frame_mode.h"
#include "platform/led_control.h"
#include "platform/adc_control.h"
#include "platform/buttons_control.h"
#include "platform/magnet_control.h"

constexpr int TLightFrameMode::PulseTime[];
constexpr int TLightFrameMode::MeasureTime[];

TLightFrameMode::TLightFrameMode(double cycleTime)
    : CycleTime(cycleTime)
{}

void TLightFrameMode::switchOn()
{
    TLedControl::enableOutput();
    TAdcControl::enableFrame();
    TAdcControl::measureLight();
    TAdcControl::set(Sound, SoundLength);
    changeDynamic();
    configureLed();
    TMagnetControl::start();
    TLedControl::start();
}

void TLightFrameMode::switchOff()
{
    TLedControl::stop();
    TMagnetControl::stop();
    TAdcControl::disableFrame();
    TLedControl::disableOutput();
}

void TLightFrameMode::onDma()
{
    TButtonsControl::startEvents();
}

int TLightFrameMode::calculateBrightness()
{
    if (MeasureLight) {
        int brightness = TAdcControl::get(1);
        TAdcControl::measureMagnet();
        MeasureLight = false;
        return brightness;
    } else {
        /*
        int amplitude = TAdcControl::get(1);
        double error = (1 - (MeasureTarget - 1.0 * amplitude) / TargetAmplitude);
        double result = 100.0 - error * 20;
        if (result > 200) result = 200;
        if (result < 80) result = 80;
        */
        TAdcControl::measureLight();
        MeasureLight = true;
        return -1;
    }
}

void TLightFrameMode::setDynamic(double frequency, double power)
{
    if (frequency!=Frequency || power!=Power) {
        Frequency = frequency;
        Power = power;
        DynamicChanged = true;
        configureLed();
    }
}

void TLightFrameMode::setBrightness(double brightness)
{
    if (brightness==Brightness) return;
    Brightness = brightness;
    configureLed();
}

void TLightFrameMode::onTimer()
{
    if (!DynamicChanged) return;
    DynamicChanged = false;
    changeDynamic();
}

void TLightFrameMode::configureLed()
{
    int ledPeriod = static_cast<int>(BaseFrequency/(Frequency-1.0/CycleTime));
    int ledPower = static_cast<int>(Brightness*ledPeriod/3.0);
    TLedControl::set(ledPeriod - 1, ledPower);
}

void TLightFrameMode::changeDynamic()
{
    int magnetPeriod = static_cast<int>(BaseFrequency/Frequency);
    int magnetTime = static_cast<int>((magnetPeriod/2.0-MeasureTimeUs)*Power);
    int magnetAdc = 0;
    for (int i = 1; i<sizeof(PulseTime)/sizeof(PulseTime[0]); ++i) {
        int lm = MeasureTime[i-1];
        int nm = MeasureTime[i];
        int lt = PulseTime[i-1];
        int nt = PulseTime[i];
        if (magnetTime<nt) {
            magnetAdc = lm+(nm-lm)*(magnetTime-lt)/(nt-lt);
            break;
        }
    }
    TMagnetControl::set(magnetPeriod, magnetTime, magnetAdc);
}

