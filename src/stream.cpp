#include "stream.h"

void TStream::enable()
{
    beginReceive();
}

size_t TStream::available()
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

size_t TStream::asyncRead(uint8_t* buffer, size_t size)
{
    size_t cnt = 0;
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

size_t TStream::read(uint8_t* buffer, size_t size)
{
    ReceiveTimeout.set(100);
    int ready = 0;
    while(ready < size || ReceiveTimeout.expired()) {
        ready += asyncRead(static_cast<uint8_t*>(buffer) + ready, size - ready);
    }
    ReceiveTimeout.unset();
    return ready;
}

size_t TStream::asyncReadUntil(uint8_t character, uint8_t* buffer, size_t size)
{
    size_t cnt = 0;
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

size_t TStream::readUntil(uint8_t character, uint8_t* buffer, size_t size)
{
    ReceiveTimeout.set(100);
    size_t ready = 0;
    while(ready < size || ReceiveTimeout.expired()) {
        ready += asyncReadUntil(character, static_cast<uint8_t*>(buffer) + ready, size - ready);
        if (buffer[ready - 1] == character) break;
    }
    ReceiveTimeout.unset();
    return ready;
}

size_t TStream::write(uint8_t byte)
{
    return asyncWrite(&byte, 1);
}

size_t TStream::asyncWrite(const uint8_t* buffer, size_t size)
{
    size_t cnt = 0;
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

size_t TStream::write(const uint8_t* buffer, size_t size)
{
    TransmitTimeout.set(100);
    size_t ready = 0;
    while(ready < size || TransmitTimeout.expired()) {
        ready += asyncWrite(static_cast<const uint8_t*>(buffer) + ready, size - ready);
    }
    return ready;
}

void TStream::bufferTransmitted(size_t size)
{
    if (TransmitRead + size == sizeof(TransmitBuffer)) {
        TransmitRead = 0;
    } else {
        TransmitRead += size;
    }

    beginTransmit();
}

void TStream::bufferReceived(size_t size)
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
        Communication.transmit(TransmitBuffer + TransmitRead, TransmitWrite - TransmitRead);
        TransmitStopped = false;
    } else if (TransmitRead > TransmitWrite) {
        TransmitStopped = false;
        Communication.transmit(TransmitBuffer + TransmitRead, sizeof(TransmitBuffer) - TransmitRead);
    } else {
        TransmitStopped = true;
    }
}

void TStream::beginReceive()
{
    if (ReceiveWrite + 1 < ReceiveRead) {
        ReceiveStopped = false;
        Communication.receive(ReceiveBuffer + ReceiveWrite, ReceiveRead - ReceiveWrite - 1);
    } else if (ReceiveWrite >= ReceiveRead) {
        ReceiveStopped = false;
        Communication.receive(ReceiveBuffer + ReceiveWrite, sizeof(ReceiveBuffer) - ReceiveWrite - 1);
    } else {
        ReceiveStopped = true;
    }
}

