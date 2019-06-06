#pragma once
#include <platform/communication_control.h>

enum class TCommandMode {
    Initial,
    ModeSelect,
    ButtonsLight,
    LedStripLight,
    PrintFormat
};

class ICommandsCallback {
public:
    virtual void onModeSelect(uint32_t mode) = 0;
    virtual void onButtonsLight(uint32_t brightness) = 0;
    virtual void onLedStripLight(uint32_t brightness) = 0;
    virtual void onPrintFormat(uint32_t format) = 0;
};

class TCommandsReceiver : public IReceiveByteCallback, ITransmitByteCallback {
public:
    explicit TCommandsReceiver(TCommunicationControl& communication)
        : Communication(communication)
        , Callback(nullptr)
    { }
    void init(ICommandsCallback* callback);
    void byteReceived(uint8_t byte) override;
    void byteTransmitted() override;

private:
    TCommunicationControl& Communication;
    ICommandsCallback* Callback;
    TCommandMode Mode{TCommandMode::Initial};
    int Number{0};
    bool CommandReady{false};
};
