#pragma once
/**
 * This is an implementation of EventGen class. It is not currently used or fully implemented, but it is intended
 * to be used as a source of random event generation such as network "disconnects", lost messages, etc.
 */

#include "AbstractStrategy.h"
#include "Network.h"
#include "SystemClock.h"

using namespace std;

class SimpleStrategy: public AbstractStrategy {
private:
    shared_ptr<SystemClock> nodeClock;
    clock_unit_t heartbeatPeriod;

public:

    SimpleStrategy(ClockType clockType, clock_unit_t heartbeatPeriod);

    ~SimpleStrategy();
    
    void logKeySharing(UUID uuid, double shortAlloc, bool shortAllocIsOne, double longAlloc, bool longAllocIsOne,
        double prevDay, double prevWeek, long double avgProv, long double avgKey, int peersChecked);

    void systemTick(Network* network) {
        for(auto &[_, n] : network->getNodes()){ nodeTick(n); }
    }

    //unused function, based on outdated ticking system
    void nodeTick(shared_ptr<Node>& node);

    //the adak algorithm used to control functionality
    void adak(Node & node, int keysToShift);
    
    //previously used to change a double to a binary expansion. It was replaced with binary operators, so this is unused
    string fractToBin(long double fract, int accuracy);

    //the second half of the control, slpit in half fro readability
    void subBlocks(Node &node, long double avgKeys, int keysToShift);
};
