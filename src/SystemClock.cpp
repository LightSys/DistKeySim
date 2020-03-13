#include <cstdint>
#include <chrono>
#include <memory>

#include "SystemClock.h"


SystemClock* SystemClock::makeClock(ClockType type){
    switch(type){
        case SIMULATION:
            return new SimulationClock();
        case REAL_SECONDS:
            return new RealSecondsClock();
        default:
            return nullptr;
    }
}


SimulationClock::SimulationClock(){
    systemCount = 0;
}

void SimulationClock::tick(clock_unit_t units){
    ++systemCount;
    if(systemCount > ROLLOVER_VALUE){
        systemCount -= ROLLOVER_VALUE;
    }
}

void SimulationClock::setTimer(unsigned int timerID, clock_unit_t units){
    clock_counter_t timer_target = systemCount;
    timer_target += ((clock_counter_t) units);
    if(timer_target > ROLLOVER_VALUE){ 
        timer_target -= ROLLOVER_VALUE;
    }
    timer_targets[timerID] = timer_target;
    timer_status[timerID] = STARTED;
}

void SimulationClock::clearTimer(unsigned int timerID){
    timer_status[timerID] = CLEARED;
}

TimerStatus SimulationClock::checkTimer(unsigned int timerID){
    
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


void RealSecondsClock::setTimer(unsigned int  timerID, clock_unit_t units){
    time_point_t timer_target = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::seconds(units);
    timer_target += delta;

    timer_targets[timerID] = timer_target;
    timer_status[timerID] = STARTED;
}

void RealSecondsClock::setPreciseTimer(unsigned int timerID, double high_res_units){
    time_point_t timer_target = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::milliseconds((unsigned int) (high_res_units * 1000.0));
    timer_target += delta;

    timer_targets[timerID] = timer_target;
    timer_status[timerID] = STARTED;
}

void RealSecondsClock::clearTimer(unsigned int timerID){
    timer_status[timerID] = CLEARED;
}

TimerStatus RealSecondsClock::checkTimer(unsigned int timerID){
    
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

