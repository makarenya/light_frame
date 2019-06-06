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

void TCommunicationControl::enable()
{
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

bool TCommunicationControl::receive(void* buffer, uint16_t length, IReceiveBufferCallback* callback)
{
    if (ReceiveBufferCallback || ReceiveByteCallback) return false;
    ReceiveBufferCallback = callback;
    ReceiveSize = length;
    dma_set_memory_address(UartDma, RxDmaChannel, reinterpret_cast<uint32_t>(buffer));
    dma_set_number_of_data(UartDma, RxDmaChannel, length);
    usart_enable_rx_dma(Uart);
    dma_enable_channel(UartDma, RxDmaChannel);
    return true;
}

bool TCommunicationControl::receive(IReceiveByteCallback* callback)
{
    if (ReceiveBufferCallback ||  ReceiveByteCallback) return false;
    ReceiveByteCallback = callback;
    if (usart_get_flag(Uart, USART_SR_RXNE)) {
        onRxNotEmpty();
    } else {
        usart_enable_rx_interrupt(Uart);
    }
    return true;
}

bool TCommunicationControl::stopReceive()
{
    if (ReceiveBufferCallback == nullptr) return false;
    usart_disable_rx_dma(Uart);
    dma_disable_channel(UartDma, RxDmaChannel);
    fireBufferReceived(ReceiveSize - dma_get_number_of_data(UartDma, RxDmaChannel));
    return true;
}

bool TCommunicationControl::transmit(const void* buffer, uint16_t length, ITransmitBufferCallback* callback)
{
    if (TransmitBufferCallback || TransmitByteCallback) return false;
    TransmitSize = length;
    TransmitBufferCallback = callback;
    dma_set_memory_address(UartDma, TxDmaChannel, reinterpret_cast<uint32_t>(buffer));
    dma_set_number_of_data(UartDma, TxDmaChannel, length);
    dma_enable_channel(UartDma, TxDmaChannel);
    USART_SR(Uart) = ~USART_SR_TC;
    usart_enable_tx_dma(Uart);
    return true;
}

bool TCommunicationControl::transmit(uint8_t byte, ITransmitByteCallback* callback)
{
    if (TransmitBufferCallback || TransmitByteCallback) return false;
    TransmitByteCallback = callback;
    USART_CR1(Uart) |= USART_CR1_TCIE;
    usart_send(Uart, byte);
    usart_enable_tx_interrupt(Uart);
    return true;
}

void TCommunicationControl::onRxDma()
{
    usart_disable_rx_dma(Uart);
    fireBufferReceived(ReceiveSize);
}

void TCommunicationControl::onTxDma()
{
    usart_disable_tx_dma(Uart);
    USART_CR1(Uart) |= USART_CR1_TCIE;
}

void TCommunicationControl::onTransferComplete()
{
    fireBufferTransmitted(TransmitSize);
}

void TCommunicationControl::onTxEmpty()
{
    fireByteTransmitted();
}

void TCommunicationControl::onRxNotEmpty()
{
    fireByteReceived(usart_recv(Uart));
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

void TCommunicationControl::fireBufferReceived(int size)
{
    if (ReceiveBufferCallback) {
        IReceiveBufferCallback* callback = ReceiveBufferCallback;
        ReceiveBufferCallback= nullptr;
        callback->bufferReceived(size);
    }
}

void TCommunicationControl::fireByteReceived(uint8_t byte)
{
    if (ReceiveByteCallback) {
        IReceiveByteCallback* callback = ReceiveByteCallback;
        ReceiveByteCallback= nullptr;
        callback->byteReceived(byte);
    }
}

void TCommunicationControl::fireBufferTransmitted(int size) {
    if (TransmitBufferCallback) {
        ITransmitBufferCallback* callback = TransmitBufferCallback;
        TransmitBufferCallback= nullptr;
        callback->bufferTransmitted(size);
    }
}

void TCommunicationControl::fireByteTransmitted() {
    if (TransmitByteCallback) {
        ITransmitByteCallback* callback = TransmitByteCallback;
        TransmitByteCallback = nullptr;
        callback->byteTransmitted();
    }
}
