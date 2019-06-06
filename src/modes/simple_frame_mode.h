#pragma once
#include "mode_control.h"

class TSimpleFrameMode : public IWorkMode {
public:
    void switchOn() override;
    void switchOff() override;
    int calculateBrightness() override;
    void onDma() override {}
    void onTimer() override {}
    void setBrightness(double brightness) override;
    void configureLed();

private:
    double Brightness{0};
};
