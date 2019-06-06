#include <platform/platform_control.h>
#include "light_frame_mode.h"
#include "platform/led_control.h"
#include "platform/adc_control.h"
#include "platform/buttons_control.h"
#include "platform/magnet_control.h"

constexpr int TLightFrameMode::PulseTime[];
constexpr int TLightFrameMode::MeasureTime[];

TLightFrameMode::TLightFrameMode(double cycleTime)
    : Sound()
    , Frequency(100)
    , CycleTime(cycleTime)
    , Power(.5)
    , Brightness(.2)
    , DynamicChanged(true)
{
    CycleTime = cycleTime;
}

void TLightFrameMode::switchOn()
{
    TLedControl::enableOutput();
    TAdcControl::enableFrame();
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
    int amplitude = TAdcControl::get(1);
    int brightness = TAdcControl::get(2);
    int ledPeriod = static_cast<int>(BaseFrequency/(Frequency-1.0/CycleTime));
    double error = (1 - 1.0 * (amplitude - MeasureTarget) / TargetAmplitude);

    if (TLedControl::getPeriod() < ledPeriod / 2) return -1;
    return brightness;
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

