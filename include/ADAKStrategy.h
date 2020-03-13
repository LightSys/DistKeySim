#ifndef LIGHTSYS_ADAK_STRATEGY_H
#define LIGHTSYS_ADAK_STRATEGY_H

#include "Network.h"

/**
* The type of ADAK Algorithm the Simulation should use
*/
enum class ADAKStrategyType { Control, LocalAverage };

class ADAKStrategy {
public:
    ADAKStrategy() {};


    void systemTick(Network* network) {
        for(auto &[_, n] : network->getNodes()){ nodeTick(n); }
    }

    /**
     * This function is called once per system tick for a node to send messages, update status,
     * and process messages in its own message queue.
     * @param network
    */
    virtual void nodeTick(shared_ptr<Node>& node) = 0;
};

#endif //LIGHTSYS_ADAK_STRATEGY_H
