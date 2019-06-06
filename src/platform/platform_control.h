#pragma once

class TPlatformControl {
public:
    static void enable();
    static void enableShield();
    static void disableShield();
    static void enableEsp();
    static void disableEsp();
    static void resetEsp();
};
