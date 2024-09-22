#pragma once

#include "Node.h"

class Network;

class AbstractStrategy {
public:
 
    virtual void systemTick(Network* network) = 0;

    //the adak algorithm used to control functionality
    virtual void adak(Node & node, int keysToShift) = 0;

    /**
     * This function is called once per system tick for a node to send messages, update status,
     * and process messages in its own message queue.
    */
    virtual void nodeTick(shared_ptr<Node>& node) = 0;

    //update the accuracy variable
    void setAccuracy(double acc) { accuracy = acc; }

protected:
    double accuracy = 0.0;
};
