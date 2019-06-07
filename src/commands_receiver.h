#pragma once
#include <platform/communication_control.h>
#include "stream.h"

enum class TCommandMode {
    Initial,
    ModeSelect,
    ButtonsLight,
    LedStripLight,
    PrintFormat
};

class TCommandsReceiver {
public:
    TCommandsReceiver() = default;
    TCommandsReceiver(const TCommandsReceiver&) = delete;
    TCommandMode update(TStream& stream);
    int value() { return Value; }

private:
    TCommandMode Mode{TCommandMode::Initial};
    int Extracted{0};
    int Value{0};
    bool CommandReady{false};
};
