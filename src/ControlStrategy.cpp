#include "ControlStrategy.h"

using namespace std;


ControlStrategy::ControlStrategy(ClockType type, clock_unit_t heartbeatPeriod){
    this->nodeClock = unique_ptr<SystemClock>(SystemClock::makeClock(type));
    this->heartbeatPeriod = heartbeatPeriod;
    this->nodeClock->setTimer(CONTROL_HEARTBEAT_TIMER, heartbeatPeriod);
}


void ControlStrategy::nodeTick(shared_ptr<Node>& node){

    nodeClock->tick();
    if(nodeClock->checkTimer(CONTROL_HEARTBEAT_TIMER) == ENDED){

        // do heartbeat:
        node->heartbeat();

        nodeClock->setTimer(CONTROL_HEARTBEAT_TIMER, heartbeatPeriod);
    }
}

