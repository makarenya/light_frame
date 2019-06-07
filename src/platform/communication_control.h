#pragma once
#include <cstdint>
#include <cstddef>
#include "hardware.h"

class ICommunicationClient {
public:
    virtual void bufferReceived(size_t size) = 0;
    virtual void bufferTransmitted(size_t size) = 0;
};

class TCommunicationControl {
public:
    TCommunicationControl(bool esp, int baud);
    void init(ICommunicationClient* client);
    bool receive(void* buffer, uint16_t length);
    bool stopReceive();
    size_t received();
    bool transmit(const void* buffer, uint16_t length);

    void onTxDma();
    void onRxDma();
    void onTransferComplete();

private:
    void enableGpio();
    void enableDma(uint32_t channel, bool rx);
    void enableUart();

    void fireTransmit(size_t size);
    void fireReceive(size_t size);

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

    ICommunicationClient* Client{};
    uint16_t ReceiveSize{};
    uint16_t TransmitSize{};
};
