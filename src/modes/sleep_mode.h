#pragma once

#include "mode_control.h"
class TSleepMode : public IWorkMode {
public:
    void switchOn() override;
    void switchOff() override;
    int calculateBrightness() override { return -1; }
    void onDma() override {}
    void onTimer() override {}
};
