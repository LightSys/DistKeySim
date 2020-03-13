#ifndef LIGHTSYS_ADAK_WEIGHTED_AVG_STRATEGY_H
#define LIGHTSYS_ADAK_WEIGHTED_AVG_STARTEGY_H

/**
 * This is an implementation of EventGen class. It is not currently used or fully implemented, but it is intended
 * to be used as a source of random event generation such as network "disconnects", lost messages, etc.
 */

#include "ADAKStrategy.h"
#include "SystemClock.h"

using namespace std;

enum WeightedAverageNodeTimer { W_AVG_HEARTBEAT_TIMER };

class WeightedAverageStrategy : public ADAKStrategy {
private:

    unique_ptr<SystemClock> nodeClock;
    clock_unit_t heartbeatPeriod;
    double diffusionRate;
    double allocationThreshold;

public:

    WeightedAverageStrategy(
        ClockType clockType, clock_unit_t heartbeatPeriod, 
        int keyspaceChunkSize, double diffusionRate, 
        double allocationThreshold
    );
    ~WeightedAverageStrategy();
    
    void processMessage(Node* node, const Message& msg);

    void nodeTick(Node* node);
};

#endif //LIGHTSYS_ADAK_WEIGHTED_AVG_STRATEGY_H
