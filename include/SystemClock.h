#ifndef ADAK_SYSTEM_CLOCK_H
#define ADAK_SYSTEM_CLOCK_H

#include <cstdint>
#include <map>
#include <chrono>
#include <memory>

enum ClockType { SIMULATION, REAL_SECONDS };

typedef std::uint64_t clock_counter_t;
typedef std::uint32_t clock_unit_t;

const std::uint64_t ROLLOVER_VALUE = 0x000000FFFFFFFFFF;
enum TimerStatus { CLEARED, STARTED, ENDED };

//class prototypes (types of SystemClock):
//class SystemClock;
class SimulationClock;
class RealSecondsClock;


class SystemClock {
public:

    SystemClock(){}

    virtual void tick(clock_unit_t units=1) = 0;
    virtual void setTimer(unsigned int timerID, clock_unit_t units) = 0;
    virtual void clearTimer(unsigned int timerID) = 0;

    virtual TimerStatus checkTimer(unsigned int timerID) = 0;

    static SystemClock* makeClock(ClockType type);
};

class SimulationClock : public SystemClock {
private:

    clock_counter_t systemCount = 0;
    std::map<unsigned int, clock_counter_t> timer_targets;
    std::map<unsigned int, TimerStatus> timer_status;

public:

    SimulationClock();

    void tick(clock_unit_t units=1);

    void setTimer(unsigned int timerID, clock_unit_t units);

    void clearTimer(unsigned int timerID);

    TimerStatus checkTimer(unsigned int timerID);
};

typedef std::chrono::high_resolution_clock::time_point time_point_t;
class RealSecondsClock : public SystemClock {
private:

    std::map<unsigned int, time_point_t> timer_targets;
    std::map<unsigned int, TimerStatus> timer_status;

public:

    RealSecondsClock(){}

    void tick(clock_unit_t units=1){} // DEAD FUNCTION

    void setTimer(unsigned int  timerID, clock_unit_t units);

    void setPreciseTimer(unsigned int timerID, double high_res_units);

    void clearTimer(unsigned int timerID);

    TimerStatus checkTimer(unsigned int timerID);
};

#endif // ADAK_SYSTEM_CLOCK_H
