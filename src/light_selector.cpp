#include <cstdlib>
#include "light_selector.h"

constexpr int TLightSelector::Intervals[];
constexpr double TLightSelector::Strip[];
constexpr int TLightSelector::Sleep[];
constexpr int TLightSelector::Blink[];
constexpr int TLightSelector::Curr[];
constexpr int TLightSelector::Other[];

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
    } else if (value > Intervals[CurrentMode + 2] && CurrentMode < ModesCount - 1) {
        suggestedMode = CurrentMode + 1;

    } else if (value < Intervals[CurrentMode] && CurrentMode > 0) {
        suggestedMode = CurrentMode - 1;
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

int TLightSelector::sleepBrightness()
{
    if (OveridedButtons > 0) {
        return OveridedButtons / 2;
    } else if (CurrentMode < 0) {
        return Sleep[1];
    }
    return Sleep[CurrentMode];
}

int TLightSelector::currentBrightness()
{
    if (OveridedButtons > 0) {
        return OveridedButtons;
    } else if (CurrentMode < 0) {
        return Curr[1];
    }
    return Curr[CurrentMode];
}

int TLightSelector::otherBrightness()
{
    if (OveridedButtons > 0) {
        return OveridedButtons * 3 / 4;
    } else if (CurrentMode < 0) {
        return Other[1];
    }
    return Other[CurrentMode];
}

int TLightSelector::blinkBrightness()
{
    if (OveridedButtons > 0) {
        return OveridedButtons * 4 / 3;
    } else if (CurrentMode < 0) {
        return Blink[1];
    }
    return Blink[CurrentMode];
}


