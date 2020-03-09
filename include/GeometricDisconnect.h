#ifndef LIGHTSYS_ADAK_GEOMETRICTDISCONNECT_H
#define LIGHTSYS_ADAK_GEOMETRICTDISCONNECT_H

const int TIMESTEP = 10;

/**
 * This is an implementation of EventGen class. This class is used to randomly
 * generate network disconnects for nodes in the network. For every tick in the
 * simulation, it will determine an event. If the event is that the node
 * disconnects, then this function will determine after how long it will
 * disconnect and then reconnect. That time length is determined using two
 * different Geometric distributions.
 *
 * Parameters:
 *    - lambda1 => parameter for one of the Geometric distributions
 *              => represents the expected time between the node being online
 *                 and then going offline
 *    - lambda2 => parameter for one of the Geometric distributions
 *             => represents the expected time between the node being offline
 *                and then going online
 *
 * Process:
 *    - For each node that is randomly disconnected
 *         (1) a Geom(lambda1, timeStep) model will be used to generate the
 *             amount of time between when the node is online and then goes
 *             offline
 *         (2) a Geom(lambda2, timeStep) model will be used to generate the
 *             amount of time between when the node is offline and goes back
 *             online
 *    Note: timeStep is a set constant that represents a set block of time.
 */

#include "EventGen.h"

class GeometricDisconnect: public EventGen {
private:

public:
    Random();
    void eventTick(Network* network, double lambda1, double lambda2);
};

#endif //LIGHTSYS_ADAK_GEOMETRICTIMEGEN_H
