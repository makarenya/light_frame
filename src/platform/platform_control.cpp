#include <libopencm3/cm3/systick.h>
#include "platform_control.h"
#include "hardware.h"

void TPlatformControl::enable()
{
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_PWR);
    rcc_periph_clock_enable(RCC_AFIO);
    gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, 0);
    rcc_periph_clock_enable(RCC_DMA1);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
    systick_set_frequency(1000, rcc_ahb_frequency);
    systick_interrupt_enable();
    systick_counter_enable();
    gpio_clear(EspEnGpio, EspEnPin);
    gpio_clear(ShieldGpio, ShieldPin);
    gpio_set(EspResetGpio, EspResetPin);
    gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON, AFIO_MAPR_TIM2_REMAP_PARTIAL_REMAP1);
    gpio_set_mode(EspEnGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, EspEnPin);
    gpio_set_mode(ShieldGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, ShieldPin);
    gpio_set_mode(EspResetGpio, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, EspResetPin);
}

void TPlatformControl::enableShield()
{
    gpio_set(ShieldGpio, ShieldPin);
}

void TPlatformControl::disableShield()
{
    gpio_clear(ShieldGpio, ShieldPin);
}

void TPlatformControl::enableEsp()
{
    gpio_set(EspEnGpio, EspEnPin);
}

void TPlatformControl::disableEsp()
{
    gpio_clear(EspEnGpio, EspEnPin);
}

void TPlatformControl::resetEsp()
{
    gpio_clear(EspResetGpio, EspResetPin);
    for(volatile int i = 0; i < 3600; ++i) {}
    gpio_set(EspResetGpio, EspResetPin);
}

