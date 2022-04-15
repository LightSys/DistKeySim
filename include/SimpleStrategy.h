#pragma once

#include "AbstractStrategy.h"
#include "Network.h"
#include "SystemClock.h"

class SimpleStrategy: public AbstractStrategy {

public:

    SimpleStrategy() { }
    ~SimpleStrategy();
    
    void systemTick(Network* network) {
        for(auto &[_, n] : network->getNodes()){ nodeTick(n); }
    }

    void logKeySharing(UUID uuid, double longAlloc, double prevWeek);

    //unused function, based on outdated ticking system
    void nodeTick(shared_ptr<Node>& node);

    //the adak algorithm used to control functionality
    void adak(Node & node, int keysToShift);
    
    //previously used to change a double to a binary expansion. It was replaced with binary operators, so this is unused
    string fractToBin(long double fract, int accuracy);
};
