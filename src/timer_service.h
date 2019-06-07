#pragma once
#include <cstdint>

class ITimerCallback {
public:
    virtual void onTimer(uint32_t timer) = 0;
};

class TTimerService {
public:
    bool addTimer(ITimerCallback* callback, uint32_t timer, int delay);
    void removeTimer(ITimerCallback* callback, uint32_t timer);
    void poll();
    uint32_t ticks() { return Ticks; }

private:
    uint32_t Ticks{0};
    uint32_t Marks[16]{};
    uint32_t Timers[16]{};
    ITimerCallback* Callbacks[16]{};
    int TimerHead{0};
};
