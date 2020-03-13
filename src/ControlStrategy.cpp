#include "ControlStrategy.h"

using namespace std;


ControlStrategy::ControlStrategy(ClockType type, clock_unit_t heartbeatPeriod){
    nodeClock = SystemClock::makeClock(type);
}


void ControlStrategy::nodeTick(Node* node){
    // TODO: implement heartbeats here
}
