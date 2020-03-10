#ifndef ADAK_SYSTEM_CLOCK_H
#define ADAK_SYSTEM_CLOCK_H

#include <cstdint>
#include <map>
#include <chrono>

enum class ClockType { SIMULATION, REAL_SECONDS };

typedef std::uint64_t clock_counter_t;
typedef std::uint32_t clock_unit_t;

const std::uint64_t ROLLOVER_VALUE = 0x000000FFFFFFFFFF;
enum TimerStatus { CLEARED, STARTED, ENDED};

class SystemClock{
public:

    SystemClock(){}

    virtual void tick(clock_unit_t units) = 0;
    virtual void setTimer(unsigned int timerID, clock_unit_t units) = 0;
    virtual void clearTimer(unsigned int timerID) = 0;

    virtual TimerStatus checkTimer(unsigned int timerID) = 0;

    static std::shared_ptr<SystemClock> makeClock(ClockType type){
        switch(type){
            case SIMULATION:
                return make_shared(new SimulationClock);
            case REAL_SECONDS;
                return make_shared(new ReamSecondsClock);
            default:
                return make_shared(nullptr);
        }
    }
};

class SimulationClock : public SystemClock {
private:

    clock_counter_t systemCount = 0;
    std::map<unsigned int, clock_counter_t> timer_targets;
    std::map<unsigned int, TimerStatus> timer_status;

public:
    SimulationClock(){
        systemCount = 0;
    }

    void tick(clock_unit_t units=1){
        ++systemCount;
        if(systemCount > ROLLOVER_VALUE){
            systemCount -= ROLLOVER_VALUE;
        }
    }

    void setTimer(unsigned int timerID, clock_unit_t units){
        clock_counter_t timer_target = systemCount;
        timer_target += ((clock_counter_t) units);
        if(timer_target > ROLLOVER_VALUE){ 
            timer_target -= ROLLOVER_VALUE;
        }
        timer_targets[timerID] = timer_target;
        timer_status[timerID] = STARTED;
    }

    void clearTimer(unsigned int timerID){
        timer_status[timerID] = CLEARED;
    }

    TimerStatus checkTimer(unsigned int timerID){
        
        auto t_done_ptr = timer_status.find(timerID);
        auto t_ptr = timer_targets.find(timerID);
        if(t_done_ptr == timer_status.end() || t_ptr == timer_targets.end()){
            return CLEARED;
        }
        
        if(t_done_ptr->second == STARTED && t_ptr->second <= systemCount){
            timer_status[timerID] = ENDED;
        }

        return timer_status[timerID];
    }
};

typedef std::chrono::high_resolution_clock::time_point time_point_t;
class RealSecondsClock : public SystemClock {
private:

    
    std::map<unsigned int, time_point_t> timer_targets;
    std::map<unsigned int, TimerStatus> timer_status;

public:


    void tick(clock_unit_t units=1){} // DEAD FUNCTION


    void setTimer(unsigned int  timerID, clock_unit_t units){
        time_point_t timer_target = std::chrono::high_resolution_clock::now();
        std::chrono::duration<unsigned int> delta(units);
        timer_target += delta;

        timer_targets[timerID] = timer_target;
        timer_status[timerID] = STARTED;
    }

    void clearTimer(unsigned int timerID){
        timer_status[timerID] = CLEARED;
    }

    TimerStatus checkTimer(unsigned int timerID){
        
        time_point_t now = std::chrono::high_resolution_clock::now();
        auto t_ptr = timer_targets.find(timerID);
        auto t_done_ptr = timer_status.find(timerID);
        if(t_done_ptr == timer_status.end() || t_ptr == timer_targets.end()){
            return CLEARED;
        }

        if(t_done_ptr->second == STARTED && t_ptr->second <= now){
            timer_status[timerID] = ENDED;
        }
        
        return timer_status[timerID];
        
    }
};

#endif // ADAK_SYSTEM_CLOCK_H