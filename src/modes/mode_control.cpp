#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
#include <platform/communication_control.h>
#include <cstdio>
#include <timer_service.h>
#include <commands_receiver.h>
#include <print.h>
#include <light_selector.h>
#include <stream.h>
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

class TModeControl : public IModeControl {
public:
    void init(TMode mode) override;
    void switchMode(TMode mode) override;
    void onDma() override;
    void onAdc() override;
    void onTimer() override;
    void tick() override;
    void poll() override;

    TCommunicationControl& communication() override { return Comm; }
    TCommunicationControl& esp() override { return Esp; }

private:
    void pollButtonStates();
    void updateButtonsBrightness();
    void receiveCommands();
    void measureAmbientLight();

    TCommunicationControl Comm{false, 115200};
    TCommunicationControl Esp{true, 115200};
    TStream Stream{Comm, Timers};
    TPressedButton PressedButton{TPressedButton::None};

    IWorkMode* Selected{nullptr};
    TMode CurrentMode{TMode::Sleep};
    TMode ReadyMode{TMode::Static};

    TTimerService Timers{};
    TTimeout UpdateBrightnessTimeout{Timers, 100};
    TLightSelector LightSelector{};
    TCommandsReceiver Receiver{};

    TSimpleFrameMode StaticMode{};
    TLightFrameMode SlowMode{2.5};
    TLightFrameMode FastMode{0.9};
    TMagnetCalibrationMode CalibrationMode;
    TSleepMode SleepMode{};

    bool CalculationProcess{false};
    int LastBrightness{0};
    int PrintFormat{0};
};


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
    Comm.init(&Stream);
    Stream.enable();
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
        CalculationProcess = true;
        break;
    case TMode::Sleep:Selected = &SleepMode;
        break;
    }
    CurrentMode = mode;
    Selected->switchOn();
    cm_enable_interrupts();

    switch (mode) {
    case TMode::Static:Stream.println("static mode enabled");
        break;
    case TMode::Slow:Stream.println("slow mode enabled");
        break;
    case TMode::Fast:Stream.println("fast mode enabled");
        break;
    case TMode::MagnetCalibration:Stream.println("calibration mode enabled");
        break;
    case TMode::Sleep:Stream.println("sleep mode enabled");
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
    pollButtonStates();
    updateButtonsBrightness();
    receiveCommands();
    measureAmbientLight();
}

void TModeControl::pollButtonStates()
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

    if (CalculationProcess && CalibrationMode.dataReady()) {
        CalibrationMode.calculate(Stream);
        CalculationProcess = false;
        switchMode(TMode::Sleep);
    }
}

void TModeControl::updateButtonsBrightness()
{
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

void TModeControl::receiveCommands()
{
    switch(Receiver.update(Stream)) {
    case TCommandMode::ModeSelect:
        if (Receiver.value()==0) {
            switchMode(TMode::Sleep);
        } else if (Receiver.value()==1) {
            switchMode(TMode::Static);
        } else if (Receiver.value()==2) {
            switchMode(TMode::Slow);
        } else if (Receiver.value()==3) {
            switchMode(TMode::Fast);
        } else if (Receiver.value()==4) {
            switchMode(TMode::MagnetCalibration);
        }
        break;
    case TCommandMode::ButtonsLight:
        LightSelector.overrideButtons(Receiver.value());
        break;
    case TCommandMode::LedStripLight:
        LightSelector.overrideStrip(Receiver.value() / 1000.0);
        break;
    case TCommandMode::PrintFormat:
        PrintFormat = Receiver.value();
        break;
    case TCommandMode::Initial:
        break;
    }
}

void TModeControl::measureAmbientLight()
{
    if (UpdateBrightnessTimeout.expired()) {
        UpdateBrightnessTimeout.set(100);
        LightSelector.update(LastBrightness);
        Selected->setBrightness(LightSelector.stripBrightness());
        if (PrintFormat == 1) {
            Stream.println(LastBrightness);
        }
    }
}
