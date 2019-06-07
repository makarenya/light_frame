#pragma once
#include <cstdint>
#include "hardware.h"

class IReceiveBufferCallback {
public:
    virtual void bufferReceived(int size) = 0;
};

class IReceiveByteCallback {
public:
    virtual void byteReceived(uint8_t byte) = 0;
};

class ITransmitBufferCallback {
public:
    virtual void bufferTransmitted(int size) = 0;
};

class ITransmitByteCallback {
public:
    virtual void byteTransmitted() = 0;
};

class TCommunicationControl {
public:
    TCommunicationControl(bool esp, int baud);
    void enable();
    bool receive(void* buffer, uint16_t length, IReceiveBufferCallback* callback);
    bool receive(IReceiveByteCallback* callback);
    bool stopReceive();
    int received();
    bool transmit(const void* buffer, uint16_t length, ITransmitBufferCallback* callback);
    bool transmit(uint8_t byte, ITransmitByteCallback* callback);

    void onTxDma();
    void onRxDma();
    void onTransferComplete();
    void onTxEmpty();
    void onRxNotEmpty();

private:
    void enableGpio();
    void enableDma(uint32_t channel, bool rx);
    void enableUart();
    void fireBufferReceived(int size);
    void fireByteReceived(uint8_t byte);
    void fireBufferTransmitted(int size);
    void fireByteTransmitted();

    const int Baud;
    const uint8_t DmaRxIrq;
    const uint8_t DmaTxIrq;
    const uint8_t UartIrq;
    const rcc_periph_clken ClkEn;
    const rcc_periph_rst ClkRst;
    const uint32_t Gpio;
    const uint32_t RxPin;
    const uint32_t TxPin;
    const uint32_t Uart;
    const uint32_t RxDmaChannel;
    const uint32_t TxDmaChannel;

    IReceiveBufferCallback* ReceiveBufferCallback{};
    IReceiveByteCallback* ReceiveByteCallback{};
    ITransmitBufferCallback* TransmitBufferCallback{};
    ITransmitByteCallback* TransmitByteCallback{};
    uint16_t ReceiveSize{};
    uint16_t TransmitSize{};
};
