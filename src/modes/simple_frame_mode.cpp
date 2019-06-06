#include "simple_frame_mode.h"
#include <platform/led_control.h>
#include <platform/adc_control.h>
#include <platform/magnet_control.h>

void TSimpleFrameMode::switchOn()
{
    TAdcControl::enableSimple();
    configureLed();
    TLedControl::enableOutput();
    TLedControl::start();
}

void TSimpleFrameMode::switchOff()
{
    TLedControl::stop();
    TLedControl::disableOutput();
    TAdcControl::disableSimple();
}

int TSimpleFrameMode::calculateBrightness()
{
    return TAdcControl::get(1);
}

void TSimpleFrameMode::setBrightness(double brightness)
{
    if (brightness == Brightness) return;
    Brightness = brightness;
    configureLed();
}

void TSimpleFrameMode::configureLed()
{
    TLedControl::set(1000, static_cast<int>(1000 * Brightness / 3));
}
