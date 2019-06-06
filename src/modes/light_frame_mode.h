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
    static constexpr int PulseTime[] = {0, 30, 200, 400, 680, 1040, 1640, 2560, 3840, 5000, 10000};
    static constexpr int MeasureTime[] = {0, 70, 270, 430, 590, 740, 900, 1040, 1130, 1170, 1190};
    static const int MeasureTarget = 600;
    static const int TargetAmplitude = 500;
    static const int SoundLength = 512;

    uint16_t Sound[SoundLength];
    double Frequency{};
    double CycleTime{};
    double Power{};
    double Brightness{};
    bool DynamicChanged{};
};
