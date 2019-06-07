#include "communication_control.h"
#include "hardware.h"

TCommunicationControl::TCommunicationControl(bool esp, int baud)
    : Baud(baud)
    , ReceiveSize(0)
    , TransmitSize(0)
    , DmaRxIrq(esp ? NVIC_DMA1_CHANNEL3_IRQ : NVIC_DMA1_CHANNEL6_IRQ)
    , DmaTxIrq(esp ? NVIC_DMA1_CHANNEL2_IRQ : NVIC_DMA1_CHANNEL7_IRQ)
    , UartIrq(esp ? NVIC_USART3_IRQ : NVIC_USART2_IRQ)
    , ClkEn(esp ? RCC_USART3 : RCC_USART2)
    , ClkRst(esp ? RST_USART3 : RST_USART2)
    , Gpio(esp ? EspGpio : CommGpio)
    , RxPin(esp ? EspRxPin : CommRxPin)
    , TxPin(esp ? EspTxPin : CommTxPin)
    , Uart(esp ? EspUart : CommUart)
    , RxDmaChannel(esp ? EspUartRxDmaChannel : CommUartRxDmaChannel)
    , TxDmaChannel(esp ? EspUartTxDmaChannel : CommUartTxDmaChannel)
{}

void TCommunicationControl::init(ICommunicationClient* client)
{
    Client = client;
    rcc_periph_clock_enable(ClkEn);
    rcc_periph_reset_pulse(ClkRst);
    nvic_enable_irq(DmaRxIrq);
    nvic_enable_irq(DmaTxIrq);
    nvic_enable_irq(UartIrq);

    enableGpio();
    enableDma(RxDmaChannel, true);
    enableDma(TxDmaChannel, false);
    enableUart();
}

bool TCommunicationControl::receive(void* buffer, uint16_t length)
{
    if (ReceiveSize > 0) return false;
    ReceiveSize = length;
    dma_set_memory_address(UartDma, RxDmaChannel, reinterpret_cast<uint32_t>(buffer));
    dma_set_number_of_data(UartDma, RxDmaChannel, length);
    usart_enable_rx_dma(Uart);
    dma_enable_channel(UartDma, RxDmaChannel);
    return true;
}

bool TCommunicationControl::stopReceive()
{
    if (ReceiveSize == 0) return false;
    usart_disable_rx_dma(Uart);
    dma_disable_channel(UartDma, RxDmaChannel);
    fireReceive(ReceiveSize - dma_get_number_of_data(UartDma, RxDmaChannel));
    return true;
}

size_t TCommunicationControl::received()
{
    if (ReceiveSize == 0) return 0;
    int num = dma_get_number_of_data(UartDma, RxDmaChannel);
    return ReceiveSize - num;
}

bool TCommunicationControl::transmit(const void* buffer, uint16_t length)
{
    if (TransmitSize > 0) return false;
    TransmitSize = length;
    dma_set_memory_address(UartDma, TxDmaChannel, reinterpret_cast<uint32_t>(buffer));
    dma_set_number_of_data(UartDma, TxDmaChannel, length);
    dma_enable_channel(UartDma, TxDmaChannel);
    USART_SR(Uart) = ~USART_SR_TC;
    usart_enable_tx_dma(Uart);
    return true;
}

void TCommunicationControl::onRxDma()
{
    usart_disable_rx_dma(Uart);
    auto size = ReceiveSize;
    ReceiveSize = 0;
    Client->bufferReceived(size);
}

void TCommunicationControl::onTxDma()
{
    usart_disable_tx_dma(Uart);
    USART_CR1(Uart) |= USART_CR1_TCIE;
}

void TCommunicationControl::onTransferComplete()
{
    fireTransmit(TransmitSize);
}

void TCommunicationControl::enableGpio()
{
    gpio_set_mode(Gpio, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, RxPin);
    gpio_set_mode(Gpio, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, TxPin);
}

void TCommunicationControl::enableDma(uint32_t channel, bool rx)
{
    if (rx) {
        dma_set_read_from_peripheral(UartDma, channel);
    } else {
        dma_set_read_from_memory(UartDma, channel);
    }
    dma_set_memory_size(UartDma, channel, DMA_CCR_MSIZE_8BIT);
    dma_set_peripheral_size(UartDma, channel, DMA_CCR_PSIZE_8BIT);
    dma_enable_memory_increment_mode(UartDma, channel);
    dma_enable_transfer_complete_interrupt(UartDma, channel);
    dma_set_peripheral_address(UartDma, channel, reinterpret_cast<uint32_t>(&USART_DR(Uart)));
    dma_set_priority(UartDma, channel, DMA_CCR_PL_LOW);
}

void TCommunicationControl::enableUart()
{
    usart_set_baudrate(Uart, Baud);
    usart_set_databits(Uart, 8);
    usart_set_stopbits(Uart, USART_STOPBITS_1);
    usart_set_parity(Uart, USART_PARITY_NONE);
    usart_set_flow_control(Uart, USART_FLOWCONTROL_NONE);
    usart_set_mode(Uart, USART_MODE_TX_RX);
    usart_enable(Uart);
}

void TCommunicationControl::fireTransmit(size_t size)
{
    TransmitSize = 0;
    Client->bufferTransmitted(size);
}

void TCommunicationControl::fireReceive(size_t size)
{
    ReceiveSize = 0;
    Client->bufferReceived(size);
}
