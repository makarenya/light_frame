//
// Created by Alexey Makarenya on 2019-06-05.
//

#include "commands_receiver.h"

void TCommandsReceiver::init(ICommandsCallback* callback)
{
    Callback = callback;
    Communication.receive(this);
}

void TCommandsReceiver::byteReceived(uint8_t byte)
{
    if (CommandReady) return;
    Communication.transmit(byte, this);
    if (Mode==TCommandMode::Initial) {
        switch (byte) {
        case 'm':Mode = TCommandMode::ModeSelect;
            break;
        case 'b':Mode = TCommandMode::ButtonsLight;
            break;
        case 'l':Mode = TCommandMode::LedStripLight;
            break;
        case 'f':Mode = TCommandMode::PrintFormat;
            break;
        case 's':Mode = TCommandMode::PrintFormat;
            Number = 0;
            CommandReady = true;
            break;
        default:break;
        }
    } else {
        if (byte=='\n') {
            CommandReady = true;
        } else if (byte>='0' && byte<='9') {
            Number = Number*10+byte-'0';
        } else {
            Mode = TCommandMode::Initial;
        }
    }
    Communication.receive(this);
}

void TCommandsReceiver::byteTransmitted()
{
    if (CommandReady) {
        switch (Mode) {
        case TCommandMode::ModeSelect:Callback->onModeSelect(Number);
            break;
        case TCommandMode::ButtonsLight:Callback->onButtonsLight(Number);
            break;
        case TCommandMode::LedStripLight:Callback->onLedStripLight(Number);
            break;
        case TCommandMode::PrintFormat:Callback->onPrintFormat(Number);
            break;
        default:break;
        }
        CommandReady = false;
        Mode = TCommandMode::Initial;
        Number = 0;
    }
}
