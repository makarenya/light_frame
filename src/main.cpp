#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#include "modes/mode_control.h"

uint8_t buffer[1024];

int main()
{
    mode_control().init(TMode::Sleep);
    while (true) {
        mode_control().poll();
    }
}

void sys_tick_handler()
{
    mode_control().tick();
}

bool checkDmaTc(uint32_t channel) {
    if (dma_get_interrupt_flag(DMA1, channel, DMA_TCIF) && (DMA_CCR(DMA1, channel) & DMA_CCR_TCIE)) {
        dma_disable_channel(DMA1, channel);
        dma_clear_interrupt_flags(DMA1, channel, DMA_TCIF);
        return true;
    }
    return false;
}

void dma1_channel1_isr()
{
    if (checkDmaTc(DMA_CHANNEL1))
        mode_control().onDma();
}

void dma1_channel2_isr()
{
    if (checkDmaTc(DMA_CHANNEL2))
        mode_control().esp().onTxDma();
}

void dma1_channel3_isr()
{
    if (checkDmaTc(DMA_CHANNEL3))
        mode_control().esp().onRxDma();
}

void dma1_channel6_isr()
{
    if (checkDmaTc(DMA_CHANNEL6))
        mode_control().communication().onRxDma();
}

void dma1_channel7_isr()
{
    if (checkDmaTc(DMA_CHANNEL7))
        mode_control().communication().onTxDma();
}

void adc1_2_isr()
{
    if (adc_eoc_injected(Adc)) {
        mode_control().onAdc();
        adc_clear_flag(Adc, ADC_SR_JSTRT | ADC_SR_JEOC);
    }
}

void tim4_isr()
{
    if (timer_interrupt_source(TIM4, TIM_SR_UIF)) {
        timer_clear_flag(TIM4, TIM_SR_UIF);
        mode_control().onTimer();
    }
}

bool checkUartTc(uint32_t uart)
{
    if (usart_get_flag(uart, USART_SR_TC) && (USART_CR1(uart) & USART_CR1_TCIE)) {
        USART_CR1(uart) &= ~USART_CR1_TCIE;
        return true;
    }
    return false;
}

void usart2_isr()
{
    if (checkUartTc(CommUart))
        mode_control().communication().onTransferComplete();
}

void usart3_isr()
{
    if (checkUartTc(EspUart))
        mode_control().esp().onTransferComplete();
}
