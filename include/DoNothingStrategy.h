#pragma once

#include "AbstractStrategy.h"
#include "Network.h"
#include "SystemClock.h"

class DoNothingStrategy: public AbstractStrategy {
private:
    shared_ptr<SystemClock> nodeClock;
    clock_unit_t heartbeatPeriod;

public:

    DoNothingStrategy() { }

    ~DoNothingStrategy() { }
    
    void systemTick(Network* network) {
        for(auto &[_, n] : network->getNodes()){ nodeTick(n); }
    }

    //unused function, based on outdated ticking system
    void nodeTick(shared_ptr<Node>& node) { }

    //the adak algorithm used to control functionality
    void adak(Node & node, int keysToShift) { }   
};
