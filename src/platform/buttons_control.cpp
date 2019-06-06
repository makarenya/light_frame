#include "buttons_control.h"

void TButtonsControl::enable()
{
    rcc_periph_clock_enable(RCC_TIM1);
    rcc_periph_reset_pulse(RST_TIM1);

    timer_set_mode(ControlTimer, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_master_mode(ControlTimer, TIM_CR2_MMS_COMPARE_OC2REF);
    timer_enable_preload(ControlTimer);
    timer_set_prescaler(ControlTimer, 17);
    timer_set_period(ControlTimer, 399);
    timer_set_oc_mode(ControlTimer, PwrBtnLedChannel, TIM_OCM_PWM1);
    timer_set_oc_mode(ControlTimer, RightBtnLedChannel, TIM_OCM_PWM1);
    timer_set_oc_mode(ControlTimer, LeftBtnLedChannel, TIM_OCM_PWM1);
    timer_set_oc_mode(ControlTimer, TIM_OC2, TIM_OCM_PWM1);
    timer_enable_oc_output(ControlTimer, PwrBtnLedChannel);
    timer_enable_oc_output(ControlTimer, RightBtnLedChannel);
    timer_enable_oc_output(ControlTimer, LeftBtnLedChannel);
    timer_enable_oc_output(ControlTimer, TIM_OC2);
    timer_enable_break_main_output(ControlTimer);

    gpio_set_mode(BtnLedGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, PwrBtnLedPin | RightBtnLedPin | LeftBtnLedPin);
    gpio_set_mode(BtnGpio, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, PwrBtnPin | RightBtnPin | LeftBtnPin);
    gpio_clear(BtnGpio, PwrBtnPin | RightBtnPin | LeftBtnPin);

    timer_enable_counter(ControlTimer);
}

void TButtonsControl::setPowerLed(int brightness)
{
    timer_set_oc_value(ControlTimer, PwrBtnLedChannel, brightness);
}

void TButtonsControl::setRightLed(int brightness)
{
    timer_set_oc_value(ControlTimer, RightBtnLedChannel, brightness);
}

void TButtonsControl::setLeftLed(int brightness)
{
    timer_set_oc_value(ControlTimer, LeftBtnLedChannel, brightness);
}

void TButtonsControl::startEvents()
{
    timer_set_oc_value(ControlTimer, TIM_OC2, 1);
}

void TButtonsControl::stopEvents()
{
    timer_set_oc_value(ControlTimer, TIM_OC2, 0);
}

bool TButtonsControl::pwr_pressed()
{
    return gpio_get(BtnGpio, PwrBtnPin);
}

bool TButtonsControl::right_pressed()
{
    return gpio_get(BtnGpio, RightBtnPin);
}

bool TButtonsControl::left_pressed()
{
    return gpio_get(BtnGpio, LeftBtnPin);
}
