#pragma once

class TMagnetControl {
public:
    static void enable();
    static void start();
    static void stop();
    static void disable();
    static void set(int period, int power, int event);
};
