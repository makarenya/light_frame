#pragma once

class TLightSelector {
public:
    bool update(int value);
    void overrideStrip(double brightness);
    void overrideButtons(int brightness);

    double stripBrightness();
    int sleepBrightness();
    int currentBrightness();
    int otherBrightness();
    int blinkBrightness();

private:
    constexpr static int Intervals[] = { 0, 100, 200, 300, 400 };
    constexpr static double Strip[] = { .001, .1, .7 };
    constexpr static int Sleep[] = { 10, 40, 200 };
    constexpr static int Blink[] = { 100, 300, 400 };
    constexpr static int Curr[] = { 50, 150, 200 };
    constexpr static int Other[] = { 30, 100, 150 };
    const static int ModesCount = sizeof(Strip)/sizeof(*Strip);

    double OveridedStrip{.5};
    int OveridedButtons{10};
    int CurrentMode{-1};
    int NewMode{-1};
    int Approves{0};
};
