#include <platform/communication_control.h>
#include <cstdio>
#include <timer_service.h>
#include <commands_receiver.h>
#include <string_builder.h>
#include <light_selector.h>
#include "mode_control.h"
#include "platform/buttons_control.h"
#include "platform/led_control.h"
#include "platform/adc_control.h"
#include "platform/magnet_control.h"
#include "platform/platform_control.h"
#include "light_frame_mode.h"
#include "magnet_calibration_mode.h"
#include "libopencm3/cm3/cortex.h"
#include "simple_frame_mode.h"
#include "sleep_mode.h"

enum class TPressedButton {
    None,
    Pwr,
    Right,
    Left
};

class TModeControl : public IModeControl, public ITransmitBufferCallback, public ICommandsCallback, public ITimerCallback {
public:
    void init(TMode mode) override;
    void switchMode(TMode mode) override;
    void onDma() override;
    void onAdc() override;
    void onTimer() override;
    void tick() override;
    void poll() override;
    void onTimer(uint32_t timer) override ;

    void onModeSelect(uint32_t mode) override;
    void onButtonsLight(uint32_t brightness) override;
    void onLedStripLight(uint32_t brightness) override;
    void onPrintFormat(uint32_t format) override;
    void bufferTransmitted(int size) override;

    TCommunicationControl& communication() override { return Communication; }
    TCommunicationControl& esp() override { return Esp; }
    TTimerService& timers() override { return Timers; }

private:
    IWorkMode* Selected{nullptr};
    TMode CurrentMode{TMode::Sleep};
    TMode ReadyMode{TMode::Static};

    TCommunicationControl Communication{false, 115200};
    TCommunicationControl Esp{true, 115200};
    TPressedButton PressedButton{TPressedButton::None};
    TSimpleFrameMode StaticMode{};
    TLightFrameMode SlowMode{2.5};
    TLightFrameMode FastMode{0.9};
    TMagnetCalibrationMode CalibrationMode;
    TCommandsReceiver Receiver{Communication};
    TSleepMode SleepMode{};
    TTimerService Timers{};
    char StringBuffer[512]{};
    TCalibrationState CalibrationState{TCalibrationState::NotStarted};
    int LastBrightness{0};
    TLightSelector LightSelector{};
    int PrintFormat{0};
};


const char SleepModeOn[] = "sleep mode enabled\n";
const char StaticModeOn[] = "static mode enabled\n";
const char SlowModeOn[] = "slow mode enabled\n";
const char FastModeOn[] = "fast mode enabled\n";
const char CalibrationModeOn[] = "calibration mode enabled\n";

static TModeControl instance{};
IModeControl& mode_control()
{
    return instance;
}

void TModeControl::init(TMode mode)
{
    Selected = nullptr;
    ReadyMode = TMode::Static;
    TPlatformControl::enable();
    TButtonsControl::enable();
    Communication.enable();
    Receiver.init(this);
    Timers.addTimer(this, 1, 500);
    switchMode(mode);
}

void TModeControl::switchMode(TMode mode)
{
    cm_disable_interrupts();
    if (CurrentMode==mode && Selected) {
        return;
    }
    if (Selected) {
        Selected->switchOff();
    }
    switch (mode) {
    case TMode::Static:Selected = &StaticMode;
        break;
    case TMode::Slow:Selected = &SlowMode;
        break;
    case TMode::Fast:Selected = &FastMode;
        break;
    case TMode::MagnetCalibration:Selected = &CalibrationMode;
        CalibrationState = TCalibrationState::Started;
        break;
    case TMode::Sleep:Selected = &SleepMode;
        break;
    }
    CurrentMode = mode;
    Selected->switchOn();
    cm_enable_interrupts();

    switch (mode) {
    case TMode::Static:Communication.transmit(StaticModeOn, sizeof(StaticModeOn)-1, this);
        break;
    case TMode::Slow:Communication.transmit(SlowModeOn, sizeof(SlowModeOn)-1, this);
        break;
    case TMode::Fast:Communication.transmit(FastModeOn, sizeof(FastModeOn)-1, this);
        break;
    case TMode::MagnetCalibration:Communication.transmit(CalibrationModeOn, sizeof(CalibrationModeOn)-1, this);
        break;
    case TMode::Sleep:Communication.transmit(SleepModeOn, sizeof(SleepModeOn)-1, this);
        break;
    }
}

void TModeControl::onDma()
{
    Selected->onDma();
}

void TModeControl::onAdc()
{
    int brightness = Selected->calculateBrightness();
    if (brightness > 0) {
        LastBrightness = brightness;
    }
}

char crlf[] = "\n";

void TModeControl::onTimer()
{
    Selected->onTimer();
}

void TModeControl::tick()
{
    Timers.poll();
}

void TModeControl::poll()
{
    if (TButtonsControl::pwr_pressed()) {
        PressedButton = TPressedButton::Pwr;
    } else if (TButtonsControl::left_pressed()) {
        PressedButton = TPressedButton::Right;
    } else if (TButtonsControl::right_pressed()) {
        PressedButton = TPressedButton::Left;
    } else if (PressedButton == TPressedButton::Pwr) {
        PressedButton = TPressedButton::None;
        if (CurrentMode==TMode::Sleep) {
            switchMode(ReadyMode);
        } else {
            switchMode(TMode::Sleep);
        }
    } else if (PressedButton == TPressedButton::Right) {
        PressedButton = TPressedButton::None;
        if (CurrentMode==TMode::Static) {
            switchMode(TMode::Slow);
        } else if (CurrentMode==TMode::Slow) {
            switchMode(TMode::Fast);
        }
    } else if (PressedButton == TPressedButton::Left) {
        PressedButton = TPressedButton::None;
        if (CurrentMode==TMode::Fast) {
            switchMode(TMode::Slow);
        } else if (CurrentMode==TMode::Slow) {
            switchMode(TMode::Static);
        }
    }

    if (CalibrationState==TCalibrationState::Started && CalibrationMode.dataReady()) {
        int length = CalibrationMode.calculate(StringBuffer, sizeof(StringBuffer));
        communication().transmit(StringBuffer, length, this);
        CalibrationState = TCalibrationState::DataReady;
    } else if (CalibrationState==TCalibrationState::Transmitted) {
        CalibrationState = TCalibrationState::NotStarted;
        switchMode(TMode::Sleep);
    }

    double pwrLight = 0;
    double rightLight = 0;
    double leftLight = 0;

    if (TButtonsControl::pwr_pressed()) {
        pwrLight = LightSelector.blinkBrightness();
    } else if (CurrentMode==TMode::Sleep) {
        pwrLight = LightSelector.sleepBrightness();
    } else if (CurrentMode==TMode::Static) {
        pwrLight = LightSelector.currentBrightness();
    } else {
        pwrLight = LightSelector.otherBrightness();
    }

    if (TButtonsControl::right_pressed()) {
        rightLight = LightSelector.blinkBrightness();
    } else if (CurrentMode==TMode::Sleep) {
        rightLight = 0;
    } else if (CurrentMode==TMode::Slow) {
        rightLight = LightSelector.currentBrightness();
    } else {
        rightLight = LightSelector.otherBrightness();
    }

    if (TButtonsControl::left_pressed()) {
        leftLight = LightSelector.blinkBrightness();
    } else if (CurrentMode==TMode::Sleep) {
        leftLight = 0;
    } else if (CurrentMode==TMode::Fast) {
        leftLight = LightSelector.currentBrightness();
    } else {
        leftLight = LightSelector.otherBrightness();
    }

    TButtonsControl::setPowerLed(pwrLight);
    TButtonsControl::setRightLed(rightLight);
    TButtonsControl::setLeftLed(leftLight);
}

void TModeControl::onModeSelect(uint32_t mode)
{
    if (mode == 0) {
        switchMode(TMode::Sleep);
    } else if (mode == 1) {
        switchMode(TMode::Static);
    } else if (mode == 2) {
        switchMode(TMode::Slow);
    } else if (mode == 3) {
        switchMode(TMode::Fast);
    } else if (mode == 4) {
        switchMode(TMode::MagnetCalibration);
    }
}

void TModeControl::onButtonsLight(uint32_t brightness)
{
    LightSelector.overrideButtons(brightness);
}

void TModeControl::onLedStripLight(uint32_t brightness)
{
    LightSelector.overrideStrip(brightness / 1000.0);
}

void TModeControl::onPrintFormat(uint32_t format)
{
    PrintFormat = format;
}

void TModeControl::bufferTransmitted(int) {
    if (CalibrationState == TCalibrationState::DataReady) {
        CalibrationState = TCalibrationState::Transmitted;
    }
}


void TModeControl::onTimer(uint32_t timer)
{
    if (timer == 1) {
        Timers.addTimer(this, 1, 100);
        LightSelector.update(LastBrightness);
        Selected->setBrightness(LightSelector.stripBrightness());
        if (PrintFormat == 1) {
            TStringBuilder builder(StringBuffer, sizeof(StringBuffer));
            builder.print(LastBrightness);
            builder.print(crlf);
            communication().transmit(StringBuffer, builder.length(), this);
        }
    }
}
