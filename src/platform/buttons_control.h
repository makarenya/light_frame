#pragma once
#include "hardware.h"

class TButtonsControl {
public:
    static void enable();

    static void setPowerLed(double brightness);
    static void setRightLed(double brightness);
    static void setLeftLed(double brightness);

    static void startEvents();
    static void stopEvents();

    static bool pwr_pressed();
    static bool right_pressed();
    static bool left_pressed();
};
