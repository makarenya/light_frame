#pragma once
#include <cstdint>

class ITimer {
public:
    virtual void poll(uint32_t ticks) = 0;
};

class TTimerService {
public:
    bool addTimer(ITimer* callback);
    void removeTimer(ITimer* callback);
    void poll();
    uint32_t ticks() { return Ticks; }

private:
    uint32_t Ticks{0};
    ITimer* Timers[16]{};
    int TimerHead{0};
};

enum class TTimerState {
    Unset,
    Set,
    Expired
};

class TTimeout : public ITimer {
public:
    explicit TTimeout(TTimerService& service, int delay = 0);
    ~TTimeout();

    void poll(uint32_t ticks) override;
    void set(int delay);
    void unset();
    bool expired() { return State == TTimerState::Expired; }

private:
    TTimerService& Service;
    TTimerState State{TTimerState::Set};
    uint32_t Mark{0};
};

