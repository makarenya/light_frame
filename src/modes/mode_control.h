#pragma once
#include <cstdint>
#include <platform/communication_control.h>

enum class TMode {
    Static,
    Slow,
    Fast,
    MagnetCalibration,
    Sleep
};


class IWorkMode {
public:
    virtual void switchOn() = 0;
    virtual void switchOff() = 0;
    virtual int calculateBrightness() = 0;
    virtual void setBrightness(double brightness) {}
    virtual void onDma() = 0;
    virtual void onTimer() = 0;
};

class IModeControl {
public:
    virtual void init(TMode mode) = 0;
    virtual void switchMode(TMode mode) = 0;
    virtual void onDma() = 0;
    virtual void onAdc() = 0;
    virtual void onTimer() = 0;

    virtual void tick() = 0;
    virtual void poll() = 0;

    virtual TCommunicationControl& communication() = 0;
    virtual TCommunicationControl& esp() = 0;
};

IModeControl& mode_control();
