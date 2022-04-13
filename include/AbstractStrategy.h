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

    string toString(bool b) {
        return b ? "true" : "false";
    }

    // Inspired by Python PEP 0485
    // https://www.python.org/dev/peps/pep-0485/#proposed-implementation
    bool areCloseEnough(long double left, long double right, long double relativeTolerance=1e-9,
                                                            long double absoluteTolerance=0.0) {
        return fabs(left-right) <= max(
            relativeTolerance * max(fabs(left), fabs(right)), absoluteTolerance);
    }

    // Use areCloseEnough to define isLessThan
    bool isLessThan(long double left, long double right) {
        if (areCloseEnough(left, right)) {
            return false;
        } else {
            return left < right;
        }
    }

    // Use areCloseEnough to define isGreaterThan
    bool isGreaterThan(long double left, long double right) {
        if (areCloseEnough(left, right)) {
            return false;
        } else {
            return left > right;
        }
    }

protected:
    double accuracy = 0.0;
};
