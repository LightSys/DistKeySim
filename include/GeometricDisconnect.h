#ifndef LIGHTSYS_ADAK_GEOMETRICTDISCONNECT_H
#define LIGHTSYS_ADAK_GEOMETRICTDISCONNECT_H

#include "SystemClock.h"

/**
 * This is an implementation of EventGen class. This class is used to randomly
 * generate network disconnects for nodes in the network. For every tick in the
 * simulation, it will randomly determine [1, 10] nodes to disconnect. ir will
 * also randomly determine after how long it will disconnect and then reconnect.
 * That time length is determined using two Geometric distributions with
 * different lamdas.
 *
 * Parameters:
 *    - lambda1 => parameter for one of the Geometric distributions
 *              => represents the expected time between the node being online
 *                 and then going offline
 *    - lambda2 => parameter for one of the Geometric distributions
 *             => represents the expected time between the node being offline
 *                and then going online
 *    - network => Network containing all of the nodes in our simulated network
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
    shared_ptr<SystemClock> clock;
    double lambda1, lamda2;
public:
    GeometricDisconnect(ClockType clockType);
    void eventTick(Network* network, double lambda1, double lambda2);
};

#endif //LIGHTSYS_ADAK_GEOMETRICTIMEGEN_H
