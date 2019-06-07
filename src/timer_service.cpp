#include "timer_service.h"

bool TTimerService::addTimer(ITimerCallback* callback, uint32_t timer, int delay)
{
    if (TimerHead >= sizeof(Timers) - 1) return false;
    Callbacks[TimerHead] = callback;
    Timers[TimerHead] = timer;
    Marks[TimerHead] = Ticks + delay;
    TimerHead++;
    return true;
}

void TTimerService::removeTimer(ITimerCallback* callback, uint32_t timer)
{
    int currentHead = 0;
    for (int i = 0; i<TimerHead; ++i) {
        if (callback != Callbacks[i] || timer != Timers[i]) {
            Callbacks[currentHead] = Callbacks[i];
            Timers[currentHead] = Timers[i];
            Marks[currentHead] = Marks[i];
            currentHead++;
        }
    }
    TimerHead = currentHead;
}

void TTimerService::poll()
{
    Ticks++;
    int currentHead = 0;
    for (int i = 0; i<TimerHead; ++i) {
        if (Marks[i]==Ticks) {
            Callbacks[i]->onTimer(Timers[i]);
        } else {
            Callbacks[currentHead] = Callbacks[i];
            Timers[currentHead] = Timers[i];
            Marks[currentHead] = Marks[i];
            currentHead++;
        }
    }
    TimerHead = currentHead;
}

