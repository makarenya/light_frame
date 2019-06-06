#pragma once

#include <platform/communication_control.h>
#include "timer_service.h"
class TStream : public IReceiveBufferCallback, public ITransmitBufferCallback, public ITimerCallback {
public:
    TStream(TCommunicationControl& communication, TTimerService& timer)
        : Communication(communication)
        , Timer(timer)
    {}
    void bufferTransmitted(int size) override;
    void bufferReceived(int size) override;
    void onTimer(uint32_t timer) override;

private:
    uint8_t ReceiveBuffer[256]{};
    uint8_t TransmitBuffer[256]{};
    int ReceiveRead{};
    int ReceiveWrite{};
    int TransmitRead{};
    int TransmitWrite{};

    TCommunicationControl& Communication;
    TTimerService& Timer;
};
