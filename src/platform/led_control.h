#pragma once

class TLedControl {
public:
    static void enable();
    static void enableOutput();
    static void disableOutput();
    static void enableSlave();
    static void disableGated();
    static void start();
    static void stop();
    static void disable();
    static void set(int period, int brightness);
};
