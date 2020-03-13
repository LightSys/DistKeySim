#ifndef LIGHTSYS_ADAK_CONTROL_STRATEGY_H
#define LIGHTSYS_ADAK_CONTROL_STRATEGY_H

#include "ADAKStrategy.h"
#include "SystemClock.h"

using namespace std;

enum ControlNodeTimer { CONTROL_HEARTBEAT_TIMER };

class ControlStrategy: public ADAKStrategy {
private:

    unique_ptr<SystemClock> nodeClock;
    clock_unit_t heartbeatPeriod;

public:

    ControlStrategy(ClockType clockType, clock_unit_t heartbeatPeriod);

    ~ControlStrategy();

    void nodeTick(shared_ptr<Node>& node);
};

#endif //LIGHTSYS_ADAK_CONTROL_STRATEGY_H
