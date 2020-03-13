#include "ControlStrategy.h"

using namespace std;


ControlStrategy::ControlStrategy(ClockType type, clock_unit_t heartbeatPeriod){
    nodeClock = shared_ptr<SystemClock>(SystemClock::makeClock(type));
}


void ControlStrategy::nodeTick(shared_ptr<Node>& node){
    // TODO: implement heartbeats here
}
