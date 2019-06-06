#pragma once
#include "mode_control.h"

enum class TCalibrationState {
    NotStarted,
    Started,
    DataReady,
    Calculated,
    Transmitted
};

class TMagnetCalibrationMode : public IWorkMode {
public:
    void switchOn() override;
    void switchOff() override;
    int calculateBrightness() override { return -1; }
    void onDma() override;
    void onTimer() override;
    int calculate(char* buffer, int len);
    bool dataReady() { return PulseWidth == 0; };

private:
    static const int Samples = 238;
    static const int Pulses = 250;

    uint16_t RawData[Samples + 5]{};
    uint16_t MeasureDelays[Pulses]{};
    int PulseWidth{0};
    int TargetValue{0};
};
