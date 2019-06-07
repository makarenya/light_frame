#include "led_control.h"
#include "platform/hardware.h"

void TLedControl::enable()
{
    // enable and reset
    rcc_periph_clock_enable(RCC_TIM3);
    rcc_periph_reset_pulse(RST_TIM3);
    // led timer work independent
    timer_set_mode(LedTimer, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_master_mode(LedTimer, TIM_CR2_MMS_COMPARE_OC4REF);
    timer_set_prescaler(LedTimer, 71);
    timer_enable_preload(LedTimer);

    timer_set_oc_mode(LedTimer, TIM_OC4, TIM_OCM_PWM2);
    timer_enable_oc_preload(LedTimer, TIM_OC4);
    timer_enable_oc_output(LedTimer, TIM_OC4);
}

void TLedControl::enableOutput()
{
    // only one channel for led strip
    timer_set_oc_mode(LedTimer, TIM_OC1, TIM_OCM_PWM1);
    timer_enable_oc_output(LedTimer, TIM_OC1);
    timer_enable_oc_preload(LedTimer, TIM_OC1);
    // init gpio for channel 1
    gpio_set_mode(LedGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, LedPin);
}

void TLedControl::disableOutput()
{
    timer_disable_oc_output(LedTimer, TIM_OC1);
}

void TLedControl::enableSlave()
{
    timer_slave_set_trigger(LedTimer, TIM_SMCR_TS_ITR1);
    timer_slave_set_mode(LedTimer, TIM_SMCR_SMS_TM);
}

void TLedControl::disableGated()
{
    timer_slave_set_mode(LedTimer, TIM_SMCR_SMS_OFF);
}

void TLedControl::start()
{
    timer_generate_event(LedTimer, TIM_EGR_UG);
    timer_enable_counter(LedTimer);
}

void TLedControl::stop()
{
    timer_disable_counter(LedTimer);
}

void TLedControl::disable()
{
    timer_disable_counter(LedTimer);
    rcc_periph_clock_disable(RCC_TIM3);
}

void TLedControl::set(int period, int brightness)
{
    timer_set_period(LedTimer, period);
    timer_set_oc_value(LedTimer, TIM_OC1, brightness);
    timer_set_oc_value(LedTimer, TIM_OC4, period * 4 / 5);
}
