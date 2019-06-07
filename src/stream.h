#pragma once

#include <platform/communication_control.h>
#include "timer_service.h"
class TStream : public IReceiveBufferCallback, public ITransmitBufferCallback {
public:
    explicit TStream(TCommunicationControl& communication, TTimerService& timer)
        : Communication(communication)
        , ReceiveTimeout(timer)
        , TransmitTimeout(timer)
    {}
    void enable();
    int available();
    int read();
    int asyncRead(uint8_t* buffer, int size);
    bool read(uint8_t* buffer, int size);
    int asyncReadUntil(uint8_t character, uint8_t* buffer, int size);
    int readUntil(uint8_t character, uint8_t* buffer, int size);
    bool write(uint8_t byte);
    int asyncWrite(uint8_t* buffer, int size);
    int write(uint8_t* buffer, int size);

    void bufferTransmitted(int size) override;
    void bufferReceived(int size) override;

private:
    void beginTransmit();
    void beginReceive();
    uint8_t ReceiveBuffer[256]{};
    uint8_t TransmitBuffer[256]{};
    volatile int ReceiveRead{};
    volatile int ReceiveWrite{};
    volatile int TransmitRead{};
    volatile int TransmitWrite{};
    volatile bool TransmitStopped{true};
    volatile bool ReceiveStopped{true};

    TCommunicationControl& Communication;
    TTimeout ReceiveTimeout;
    TTimeout TransmitTimeout;
};
