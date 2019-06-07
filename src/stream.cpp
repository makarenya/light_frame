#include "stream.h"

void TStream::enable()
{
    beginReceive();
}

int TStream::available()
{
    if (!ReceiveStopped && Communication.received()) {
        Communication.stopReceive();
    }
    return (ReceiveWrite - ReceiveRead) % sizeof(ReceiveBuffer);
}

int TStream::read()
{
    uint8_t result = 0;
    if (asyncRead(&result, 1) == 1) {
        return result;
    }
    return -1;
}

int TStream::asyncRead(uint8_t* buffer, int size)
{
    int cnt = 0;
    available();
    while(ReceiveRead != ReceiveWrite && cnt < size) {
        (*buffer++) = ReceiveBuffer[ReceiveRead];
        ReceiveRead = (ReceiveRead + 1) % sizeof(ReceiveBuffer);
        cnt++;
    }
    if (ReceiveStopped) {
        beginReceive();
    }
    return cnt;
}

bool TStream::read(uint8_t* buffer, int size)
{
    ReceiveTimeout.set(100);
    int ready = 0;
    while(ready < size || ReceiveTimeout.expired()) {
        ready += asyncRead(static_cast<uint8_t*>(buffer) + ready, size - ready);
    }
    ReceiveTimeout.unset();
    return !ReceiveTimeout.expired();
}

int TStream::asyncReadUntil(uint8_t character, uint8_t* buffer, int size)
{
    int cnt = 0;
    available();
    int current = -1;
    while (ReceiveRead != ReceiveWrite && cnt < size && current != character) {
        current = (*buffer++) = ReceiveBuffer[ReceiveRead];
        ReceiveRead = (ReceiveRead + 1) % sizeof(ReceiveBuffer);
        cnt++;
    }
    if (ReceiveStopped) {
        beginReceive();
    }
    return cnt;
}

int TStream::readUntil(uint8_t character, uint8_t* buffer, int size)
{
    ReceiveTimeout.set(100);
    int ready = 0;
    while(ready < size || ReceiveTimeout.expired()) {
        ready += asyncReadUntil(character, static_cast<uint8_t*>(buffer) + ready, size - ready);
        if (buffer[ready - 1] == character) break;
    }
    ReceiveTimeout.unset();
    return ready;
}

bool TStream::write(uint8_t byte)
{
    return asyncWrite(&byte, 1) == 1;
}

int TStream::asyncWrite(uint8_t* buffer, int size)
{
    int cnt = 0;
    while(TransmitWrite != (TransmitRead + sizeof(TransmitBuffer) - 1) % sizeof(TransmitBuffer) && cnt < size) {
        TransmitBuffer[TransmitWrite] = (*buffer++);
        TransmitWrite = (TransmitWrite + 1) % sizeof(TransmitBuffer);
        cnt++;
    }
    if (TransmitStopped) {
        beginTransmit();
    }
    return cnt;
}

int TStream::write(uint8_t* buffer, int size)
{
    TransmitTimeout.set(100);
    int ready = 0;
    while(ready < size || TransmitTimeout.expired()) {
        ready += asyncWrite(static_cast<uint8_t*>(buffer) + ready, size - ready);
    }
    return ready;
}

void TStream::bufferTransmitted(int size)
{
    if (TransmitRead + size == sizeof(TransmitBuffer)) {
        TransmitRead = 0;
    } else {
        TransmitRead += size;
    }

    beginTransmit();
}

void TStream::bufferReceived(int size)
{
    if (ReceiveWrite + size == sizeof(ReceiveBuffer)) {
        ReceiveWrite = 0;
    } else {
        ReceiveWrite += size;
    }

    beginReceive();
}

void TStream::beginTransmit()
{
    if (TransmitRead < TransmitWrite) {
        Communication.transmit(TransmitBuffer + TransmitRead, TransmitWrite - TransmitRead, this);
        TransmitStopped = false;
    } else if (TransmitRead > TransmitWrite) {
        TransmitStopped = false;
        Communication.transmit(TransmitBuffer + TransmitRead, sizeof(TransmitBuffer) - TransmitRead, this);
    } else {
        TransmitStopped = true;
    }
}

void TStream::beginReceive()
{
    if (ReceiveWrite < ReceiveRead - 1) {
        ReceiveStopped = false;
        Communication.receive(ReceiveBuffer + ReceiveWrite, ReceiveRead - ReceiveWrite - 1, this);
    } else if (ReceiveWrite >= ReceiveRead) {
        ReceiveStopped = false;
        Communication.receive(ReceiveBuffer + ReceiveWrite, sizeof(ReceiveBuffer) - ReceiveWrite, this);
    } else {
        ReceiveStopped = true;
    }
}

