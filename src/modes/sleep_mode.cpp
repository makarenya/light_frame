#include <platform/dream_time.h>
#include <platform/led_control.h>
#include <platform/magnet_control.h>
#include <platform/adc_control.h>
#include <platform/platform_control.h>
#include "sleep_mode.h"

void TSleepMode::switchOn()
{
    TAdcControl::disable();
    TMagnetControl::disable();
    TLedControl::disable();
    TDreamTime::enable();
    TPlatformControl::disableShield();
}

void TSleepMode::switchOff()
{
    TPlatformControl::enableShield();
    TDreamTime::disable();
    TLedControl::enable();
    TMagnetControl::enable();
    TAdcControl::enable();
}
