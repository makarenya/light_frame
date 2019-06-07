#include "timer_service.h"

bool TTimerService::addTimer(ITimer* timer)
{
    if (TimerHead >= sizeof(Timers) - 1) return false;
    Timers[TimerHead] = timer;
    TimerHead++;
    return true;
}

void TTimerService::removeTimer(ITimer* timer)
{
    int currentHead = 0;
    for (int i = 0; i<TimerHead; ++i) {
        if (timer != Timers[i]) {
            Timers[currentHead] = Timers[i];
            currentHead++;
        }
    }
    TimerHead = currentHead;
}

void TTimerService::poll()
{
    Ticks++;
    for (int i = 0; i<TimerHead; ++i) {
        Timers[i]->poll(Ticks);
    }
}

TTimeout::TTimeout(TTimerService& service, int delay)
        : Service(service)
{
    Service.addTimer(this);
    if (delay > 0) set(delay);
}

TTimeout::~TTimeout()
{
    Service.removeTimer(this);
}

void TTimeout::poll(uint32_t ticks)
{
    if (State == TTimerState::Set && Mark==ticks) {
        State = TTimerState::Expired;
    }
}

void TTimeout::set(int delay)
{
    Mark = Service.ticks()+delay;
    State = TTimerState::Set;
}

void TTimeout::unset()
{
    State = TTimerState::Unset;
}
