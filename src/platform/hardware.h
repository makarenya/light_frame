#pragma once

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

// main frame controls
const uint32_t PlusGpio = GPIOA;
const uint32_t PlusPin = GPIO15;
const tim_oc_id  PlusChannel = TIM_OC1;
const uint32_t MinusGpio = GPIOB;
const uint32_t MinusPin = GPIO6;
const tim_oc_id MinusChannel = TIM_OC1;
const uint32_t LedGpio = GPIOA;
const uint32_t LedPin = GPIO6;
const tim_oc_id LedChannel = TIM_OC1;

// modules enable pins
const uint32_t ShieldGpio = GPIOA;
const uint32_t ShieldPin = GPIO4;
const uint32_t EspEnGpio = GPIOA;
const uint32_t EspEnPin = GPIO5;
const uint32_t EspResetGpio = GPIOB;
const uint32_t EspResetPin = GPIO1;

// button leds
const uint32_t BtnLedGpio = GPIOA;
const uint32_t PwrBtnLedPin = GPIO8;
const tim_oc_id PwrBtnLedChannel = TIM_OC1;
const uint32_t LeftBtnLedPin = GPIO10;
const tim_oc_id LeftBtnLedChannel = TIM_OC3;
const uint32_t RightBtnLedPin = GPIO11;
const tim_oc_id RightBtnLedChannel = TIM_OC4;

// buttons input
const uint32_t BtnGpio = GPIOB;
const uint32_t PwrBtnPin = GPIO9;
const uint32_t LeftBtnPin = GPIO3;
const uint32_t RightBtnPin = GPIO7;

// uarts
const uint32_t CommGpio = GPIOA;
const uint32_t CommTxPin = GPIO2;
const uint32_t CommRxPin = GPIO3;
const uint32_t EspGpio = GPIOB;
const uint32_t EspTxPin = GPIO10;
const uint32_t EspRxPin = GPIO11;

// analog inputs
const uint32_t MagAdcGpio = GPIOA;
const uint32_t MagAdcPin = GPIO0;
const uint32_t MagAdcChannel = ADC_CHANNEL0;
const uint32_t MicAdcGpio = GPIOA;
const uint32_t MicAdcPin = GPIO1;
const uint32_t MicAdcChannel = ADC_CHANNEL1;
const uint32_t LightAdcGpio = GPIOA;
const uint32_t LightAdcPin = GPIO7;
const uint32_t LightAdcChannel = ADC_CHANNEL7;

// devices
const uint32_t Adc = ADC1;
const uint32_t AdcDmaChannel = DMA_CHANNEL1;
const uint32_t ControlTimer = TIM1;
const uint32_t PlusTimer = TIM2;
const uint32_t MinusTimer = TIM4;
const uint32_t LedTimer = TIM3;
const uint32_t CommUart = USART2;
const uint32_t CommUartTxDmaChannel = DMA_CHANNEL7;
const uint32_t CommUartRxDmaChannel = DMA_CHANNEL6;
const uint32_t EspUart = USART3;
const uint32_t EspUartTxDmaChannel = DMA_CHANNEL2;
const uint32_t EspUartRxDmaChannel = DMA_CHANNEL3;
const uint32_t AdcDma = DMA1;
const uint32_t UartDma = DMA1;
