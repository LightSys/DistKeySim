#include "ControlStrategy.h"

using namespace std;


ControlStrategy::ControlStrategy(ClockType type){
    nodeClock = SystemClock::makeClock(type);
}


void ControlStrategy::nodeTick(Node* node){
    // TODO: implement heartbeats here
}

#endif //LIGHTSYS_ADAK_CONTROL_STRATEGY_H
