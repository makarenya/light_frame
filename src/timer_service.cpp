#include "timer_service.h"

void TTimerService::addTimer(ITimerCallback* callback, uint32_t timer, int delay)
{
    Callbacks[TimerHead] = callback;
    Timers[TimerHead] = timer;
    Marks[TimerHead] = Ticks + delay;
    TimerHead++;
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
