#pragma once
#include <cstdint>

class TAdcControl {
public:
    static void enable();
    static void disable();
    static void enableFrame();
    static void measureLight();
    static void measureMagnet();
    static void disableFrame();
    static void enableCalibration();
    static void disableCalibration();
    static void enableSimple();
    static void disableSimple();
    static void set(uint16_t* buffer, uint16_t length);
    static uint32_t get(int reg);
};
