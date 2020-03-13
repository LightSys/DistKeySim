#include "ControlStrategy.h"
#include "SystemClock.h"

using namespace std;


ControlStrategy::ControlStrategy(ClockType type, clock_unit_t heartbeatPeriod){
    this->nodeClock = make_shared(SystemClock::makeClock(type));
    this->heartbeatPeriod = heartbeatPeriod;
    this->nodeClock->setTimer(HEARTBEAT_TIMER, heartbeatPeriod);
}


void ControlStrategy::nodeTick(shared_ptr<Node>& node){

    nodeClock.tick();
    if(nodeClock->checkTimer(HEARTBEAT_TIMER) == ENDED){

        // do heartbeat:
        node->heartbeat();

        nodeClock->setTimer(HEARTBEAT_TIMER, heartbeatPeriod);
    }
}

#endif //LIGHTSYS_ADAK_CONTROL_STRATEGY_H
