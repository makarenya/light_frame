#pragma once
#include "hardware.h"

class TButtonsControl {
public:
    static void enable();

    static void setPowerLed(int brightness);
    static void setRightLed(int brightness);
    static void setLeftLed(int brightness);

    static void startEvents();
    static void stopEvents();

    static bool pwr_pressed();
    static bool right_pressed();
    static bool left_pressed();
};
