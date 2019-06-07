#pragma once
#include <platform/communication_control.h>
#include <cstddef>
#include "timer_service.h"
#include "print.h"

class TStream : public TPrint, public ICommunicationClient {
public:
    explicit TStream(TCommunicationControl& communication, TTimerService& timer)
        : Communication(communication)
        , ReceiveTimeout(timer)
        , TransmitTimeout(timer)
    {}
    TStream(const TStream&) = delete;
    void enable();
    size_t available();
    int read();
    size_t asyncRead(uint8_t* buffer, size_t size);
    size_t read(uint8_t* buffer, size_t size);
    size_t asyncReadUntil(uint8_t character, uint8_t* buffer, size_t size);
    size_t readUntil(uint8_t character, uint8_t* buffer, size_t size);
    size_t write(uint8_t byte) override ;
    size_t asyncWrite(const uint8_t* buffer, size_t size);
    size_t write(const uint8_t* buffer, size_t size) override;

    void bufferTransmitted(size_t size) override;
    void bufferReceived(size_t size) override;

private:
    void beginTransmit();
    void beginReceive();
    uint8_t ReceiveBuffer[256]{};
    uint8_t TransmitBuffer[256]{};
    volatile size_t ReceiveRead{};
    volatile size_t ReceiveWrite{};
    volatile size_t TransmitRead{};
    volatile size_t TransmitWrite{};
    volatile bool TransmitStopped{true};
    volatile bool ReceiveStopped{true};

    TCommunicationControl& Communication;
    TTimeout ReceiveTimeout;
    TTimeout TransmitTimeout;
};
