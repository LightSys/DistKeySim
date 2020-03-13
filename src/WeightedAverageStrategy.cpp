#include "WeightedAverageStrategy.h"
#include "SystemClock.h"

using namespace std;


WeightedAverageStrategy::WeightedAverageStrategy(ClockType type, clock_unit_t heartbeatPeriod){
    this->nodeClock = SystemClock::makeClock(type);
    this->heartbeatPeriod = heartbeatPeriod;
    this->nodeClock->setTimer(HEARTBEAT_TIMER, heartbeatPeriod);
}

void WeightedAverageStrategy::nodeTick(Node* node){

    nodeClock.tick();

    if(nodeClock->checkTimer(HEARTBEAT_TIMER) == ENDED){

        //check new messages:
        queue<Messages> newMessages = node->getReceivedMessages();
        while(!newMessages.empty()){
            processMessage(newMessages.front());
            newMessages.pop();
        }

        node->heartbeat();
        nodeClock->setTimer(HEARTBEAT_TIMER, heartbeatPeriod);
    }
}

void processMessage(Node* node, const Message& msg){

    
}

#endif //LIGHTSYS_ADAK_CONTROL_STRATEGY_H
