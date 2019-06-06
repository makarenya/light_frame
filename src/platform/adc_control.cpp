#include "adc_control.h"
#include "hardware.h"

void TAdcControl::enable()
{
    rcc_periph_clock_enable(RCC_ADC1);
    rcc_periph_reset_pulse(RST_ADC1);

    gpio_set_mode(MagAdcGpio, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, MagAdcPin);
    gpio_set_mode(MicAdcGpio, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, MicAdcPin);
    gpio_set_mode(LightAdcGpio, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, LightAdcPin);

    dma_set_read_from_peripheral(AdcDma, AdcDmaChannel);
    dma_set_memory_size(AdcDma, AdcDmaChannel, DMA_CCR_MSIZE_16BIT);
    dma_set_peripheral_size(AdcDma, AdcDmaChannel, DMA_CCR_PSIZE_16BIT);
    dma_enable_memory_increment_mode(AdcDma, AdcDmaChannel);
    dma_enable_transfer_complete_interrupt(AdcDma, AdcDmaChannel);
    dma_set_peripheral_address(AdcDma, AdcDmaChannel, reinterpret_cast<uint32_t>(&ADC_DR(Adc)));

    adc_enable_scan_mode(Adc);
    adc_set_single_conversion_mode(Adc);
    adc_set_sample_time_on_all_channels(Adc, ADC_SMPR_SMP_13DOT5CYC);
    adc_set_right_aligned(Adc);
    adc_enable_dma(Adc);
    adc_power_on(Adc);

    for(volatile int i = 0; i < 7200; ++i) {}

    adc_reset_calibration(Adc);
    adc_calibrate(Adc);
}

void TAdcControl::disable()
{
    adc_power_off(Adc);
    nvic_disable_irq(NVIC_DMA1_CHANNEL1_IRQ);
    rcc_periph_clock_disable(RCC_ADC1);
}

void TAdcControl::enableFrame()
{
    nvic_enable_irq(NVIC_ADC1_2_IRQ);
    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);
    adc_enable_eoc_interrupt_injected(Adc);

    uint8_t injected[] = { MagAdcChannel, LightAdcChannel };
    adc_set_injected_sequence(Adc, sizeof(injected), injected);
    uint8_t regular[] = { MicAdcChannel };
    adc_set_regular_sequence(Adc, sizeof(regular), regular);

    adc_enable_external_trigger_regular(Adc, ADC_CR2_EXTSEL_TIM1_CC2);
    adc_enable_external_trigger_injected(Adc, ADC_CR2_JEXTSEL_TIM2_TRGO);
}

void TAdcControl::disableFrame()
{
    dma_disable_channel(AdcDma, AdcDmaChannel);
    adc_disable_external_trigger_regular(Adc);
    adc_disable_external_trigger_injected(Adc);
    nvic_disable_irq(NVIC_ADC1_2_IRQ);
}

void TAdcControl::enableCalibration()
{
    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);
    uint8_t regular[] = { MagAdcChannel };
    adc_set_regular_sequence(Adc, sizeof(regular), regular);
    adc_enable_external_trigger_regular(Adc, ADC_CR2_EXTSEL_TIM3_TRGO);
}

void TAdcControl::disableCalibration()
{
    dma_disable_channel(AdcDma, AdcDmaChannel);
    adc_disable_external_trigger_regular(Adc);
}

void TAdcControl::enableSimple()
{
    nvic_enable_irq(NVIC_ADC1_2_IRQ);
    adc_enable_eoc_interrupt_injected(Adc);

    uint8_t injected[] = { LightAdcChannel };
    adc_set_injected_sequence(Adc, sizeof(injected), injected);

    adc_enable_external_trigger_injected(Adc, ADC_CR2_JEXTSEL_TIM3_CC4);
}

void TAdcControl::disableSimple()
{
    adc_disable_external_trigger_injected(Adc);
    nvic_disable_irq(NVIC_ADC1_2_IRQ);
}

void TAdcControl::set(uint16_t* buffer, uint16_t length)
{
    dma_set_memory_address(AdcDma, AdcDmaChannel, reinterpret_cast<uint32_t>(buffer));
    dma_set_number_of_data(AdcDma, AdcDmaChannel, length);
    dma_enable_channel(AdcDma, AdcDmaChannel);
}

uint32_t TAdcControl::get(int reg)
{
    return adc_read_injected(Adc, reg);
}

