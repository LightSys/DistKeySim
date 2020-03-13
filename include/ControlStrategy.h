#ifndef LIGHTSYS_ADAK_CONTROL_STRATEGY_H
#define LIGHTSYS_ADAK_CONTROL_STARTEGY_H

/**
 * This is an implementation of EventGen class. It is not currently used or fully implemented, but it is intended
 * to be used as a source of random event generation such as network "disconnects", lost messages, etc.
 */

#include "ADAKStrategy.h"
#include "SystemClock.h"

using namespace std;

enum NodeTimer { HEARTBEAT_TIMER };

class ControlStrategy: public ADAKStrategy {
private:

    shared_ptr<SystemClock> nodeClock;
    clock_unit_t heartbeatPeriod;

public:

    ControlStrategy(ClockType clockType, clock_unit_t heartbeatPeriod);

    ~ControlStrategy();

    void nodeTick(shared_ptr<Node>& node);
};

#endif //LIGHTSYS_ADAK_CONTROL_STRATEGY_H
