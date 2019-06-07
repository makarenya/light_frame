#pragma once
//
// Created by Alexey Makarenya on 2019-06-04.
//

#include "mode_control.h"

class TLightFrameMode : public IWorkMode {
public:
    explicit TLightFrameMode(double cycleTime);
    void switchOn() override;
    void switchOff() override;
    void onDma() override;
    int calculateBrightness() override;

    void setDynamic(double frequency, double power);
    void setBrightness(double brightness) override;
    void onTimer() override;

private:
    void configureLed();
    void changeDynamic();
    static const int BaseFrequency = 1000000;
    static const int MeasureTimeUs = 1200;
    static const int MeasureTarget = 1786;
    static constexpr int MeasureTime[] = { 0, 40, 186, 334, 462, 629, 763, 864, 966, 1077, 1137, 1188, 1189 };
    static constexpr int PulseTime[] = { 0, 20, 160, 340, 540, 880, 1240, 1600, 2140, 2980, 3740, 4920, 5500 };
    static const int TargetAmplitude = 686;
    static const int SoundLength = 512;

    uint16_t Sound[SoundLength]{};
    double Frequency{100};
    double CycleTime{};
    double Power{1.0};
    double Brightness{.2};
    bool DynamicChanged{true};
    bool MeasureLight{true};
};
