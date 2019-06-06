#include "magnet_control.h"
#include "hardware.h"

void TMagnetControl::enable()
{
    // enable and reset
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_reset_pulse(RST_TIM2);

    // plus timer runs as master
    timer_set_mode(PlusTimer, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_master_mode(PlusTimer, TIM_CR2_MMS_COMPARE_OC2REF);
    timer_slave_set_trigger(PlusTimer, TIM_SMCR_TS_ITR3);
    timer_slave_set_mode(PlusTimer, TIM_SMCR_SMS_TM);
    timer_set_prescaler(PlusTimer, 71);
    timer_one_shot_mode(PlusTimer);
    // first channel for shield, second for control minus timer
    timer_set_oc_mode(PlusTimer, TIM_OC1, TIM_OCM_PWM2);
    timer_set_oc_mode(PlusTimer, TIM_OC2, TIM_OCM_PWM2);
    // init gpio for channel 1
    gpio_set_mode(PlusGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, PlusPin);

    // enable and reset
    rcc_periph_clock_enable(RCC_TIM4);
    rcc_periph_reset_pulse(RST_TIM4);

    // minus timer runs in one shot mode for supplementary magnet control
    timer_enable_irq(MinusTimer, TIM_DIER_UIE);
    timer_set_mode(MinusTimer, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_master_mode(MinusTimer, TIM_CR2_MMS_UPDATE);
    timer_set_prescaler(MinusTimer, 71);
    // first channel for shield, second for control adc
    timer_set_oc_mode(MinusTimer, TIM_OC1, TIM_OCM_PWM1);
    // init gpio for channel 1
    gpio_set_mode(MinusGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, MinusPin);
    stop();
}

void TMagnetControl::start()
{
    nvic_enable_irq(NVIC_TIM4_IRQ);
    timer_generate_event(PlusTimer, TIM_EGR_UG);
    timer_generate_event(MinusTimer, TIM_EGR_UG);
    timer_enable_counter(MinusTimer);
    timer_enable_oc_output(PlusTimer, TIM_OC1);
    timer_enable_oc_output(MinusTimer, TIM_OC1);
}

void TMagnetControl::stop()
{
    timer_disable_oc_output(MinusTimer, TIM_OC1);
    timer_disable_oc_output(PlusTimer, TIM_OC1);
    nvic_disable_irq(NVIC_TIM4_IRQ);
    timer_disable_counter(PlusTimer);
    timer_disable_counter(MinusTimer);
}

void TMagnetControl::disable()
{
    timer_disable_counter(MinusTimer);
    timer_disable_counter(PlusTimer);
    rcc_periph_clock_disable(RCC_TIM4);
    rcc_periph_clock_disable(RCC_TIM2);
}

void TMagnetControl::set(int period, int power, int event)
{
    timer_set_period(PlusTimer, (period - 1) / 2 + power - 1);
    timer_set_oc_value(PlusTimer, TIM_OC1, (period - 1) / 2);
    timer_set_oc_value(PlusTimer, TIM_OC2, power + event);

    timer_set_period(MinusTimer, period - 1);
    timer_set_oc_value(MinusTimer, TIM_OC1, power);
}
