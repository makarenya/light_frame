#include <cstdlib>
#include "light_selector.h"

constexpr int TLightSelector::Intervals[];
constexpr double TLightSelector::Strip[];
constexpr double TLightSelector::Buttons[];

bool TLightSelector::update(int value)
{
    int suggestedMode = -1;
    if (CurrentMode < 0) {
        int best = -1;
        int error = 1000000;
        for(int i = 0; i < ModesCount; i++) {
            int mid = (Intervals[i] + Intervals[i + 2]) / 2;
            if (abs(mid - value) < error) {
                error = abs(mid - value);
                best = i;
            }
        }
        CurrentMode = best;
    } else if (value > Intervals[CurrentMode * 2 + 1] && CurrentMode < ModesCount - 1) {
        suggestedMode = CurrentMode + 1;

    } else if (value < Intervals[CurrentMode * 2] && CurrentMode > 0) {
        suggestedMode = CurrentMode - 1;
    } else {
        Approves = 0;
    }
    if (NewMode == suggestedMode && suggestedMode >= 0) {
        if (Approves >= 2) {
            CurrentMode = NewMode;
            NewMode = -1;
            Approves = 0;
        } else {
            Approves++;
        }
    } else {
        Approves = 0;
        NewMode = suggestedMode;
    }
    return false;
}

void TLightSelector::overrideStrip(double brightness)
{
    OveridedStrip = brightness;
}

void TLightSelector::overrideButtons(int brightness)
{
    OveridedButtons = brightness;
}

double TLightSelector::stripBrightness()
{
    if (OveridedStrip > 0) {
        return OveridedStrip;
    } else if (CurrentMode < 0) {
        return Strip[1];
    }
    return Strip[CurrentMode];
}

double TLightSelector::sleepBrightness()
{
    return 0.03;
    //return buttonsBase() / 3;
}

double TLightSelector::currentBrightness()
{
    return buttonsBase();
}

double TLightSelector::otherBrightness()
{

    return buttonsBase() / 3;
}

double TLightSelector::blinkBrightness()
{
    return buttonsBase() * 2;
}

double TLightSelector::buttonsBase()
{
    if (OveridedButtons > 0) {
        return OveridedButtons;
    } else if (CurrentMode < 0) {
        return Buttons[1];
    } else {
        return Buttons[CurrentMode];
    }
}
