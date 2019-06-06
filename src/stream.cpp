#include "stream.h"

void TStream::bufferTransmitted(int size)
{
    if (TransmitRead == sizeof(TransmitBuffer) && TransmitWrite > 0) {
        //Communication.transmit(TransmitBuffer, TransmitWrite);
    } else if (TransmitWrite < TransmitRead) {
    }
}

void TStream::bufferReceived(int size)
{
    ReceiveWrite += size;
    if (ReceiveRead > ReceiveWrite + 1) {
        //Communication.receive(ReceiveBuffer + ReceiveWrite, ReceiveRead - ReceiveWrite - 1);
    } else if (ReceiveRead <= ReceiveWrite) {
        //Communication.receive(ReceiveBuffer + ReceiveWrite, sizeof(ReceiveBuffer) - ReceiveWrite);
    }
}

void TStream::onTimer(uint32_t timer)
{
    Communication.stopReceive();
    Timer.addTimer(this, 0, 10);
}

