#include "commands_receiver.h"

TCommandMode TCommandsReceiver::update(TStream& stream)
{
    uint8_t buffer[256];
    size_t received = stream.asyncRead(buffer, sizeof(received));
    for (size_t i = 0; i<received; ++i) {
        uint8_t byte = buffer[i];
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
                Mode = TCommandMode::Initial;
                Value = 0;
                return TCommandMode::PrintFormat;
            default:break;
            }
        } else {
            if (byte=='\n') {
                auto mode = Mode;
                Mode = TCommandMode::Initial;
                Value = Extracted;
                Extracted = 0;
                stream.write(buffer, received);
                return mode;
            } else if (byte>='0' && byte<='9') {
                Extracted = Extracted*10+byte-'0';
            } else {
                Mode = TCommandMode::Initial;
            }
        }
    }
    if (received > 0) {
        stream.write(buffer, received);
    }
    return TCommandMode::Initial;
}
