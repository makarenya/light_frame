#pragma once

class TLightSelector {
public:
    bool update(int value);
    void overrideStrip(double brightness);
    void overrideButtons(int brightness);

    double stripBrightness();
    double sleepBrightness();
    double currentBrightness();
    double otherBrightness();
    double blinkBrightness();

private:
    double buttonsBase();
    // 1350 // 2300 // 3100
    constexpr static int Intervals[] = { 0, 2000, 2400, 3400, 3400, 4000 };
    constexpr static double Strip[] = { .05, .2, .7 };
    constexpr static double Buttons[] = { 0.025, 0.15, 0.75 };
    const static int ModesCount = sizeof(Strip)/sizeof(*Strip);

    double OveridedStrip{0};
    int OveridedButtons{0};
    int CurrentMode{-1};
    int NewMode{-1};
    int Approves{0};
};
