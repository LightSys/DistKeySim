#ifndef ADAK_KEYING_GEOMETRICTDISCONNECT_H
#define ADAK_KEYING_GEOMETRICTDISCONNECT_H

#include <map>
#include <ctime>
#include <random>
#include "UUID.h"
#include "SystemClock.h"
#include "EventGen.h"

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


class GeometricDisconnect : public EventGen {
private:
    shared_ptr<SystemClock> clock;
    double lambda1, lamda2, lambda3;

    //node id -> index into the clock timer array for the current timer
    // (time to start)
    std::map<UUID, int> goOfflineTimer;
    //node id -> when the node will come back oneline timer index in the Clock class
    std::map<UUID, int> backOnlineTimer;

    // d1 is the model used to randomly generate the amount of time from the
    // current time step when the node will go offline
    geometric_distribution<> *d1;
    // d2 is the model used to randomly generate the amount of time the current
    // node will remain offline
    geometric_distribution<> *d2;

    mt19937 *gen;
public:
    GeometricDisconnect(ClockType clockType, double lambda1, double lambda2);

    /**
     * Event tick causes one tick in the events, meaning that nodes could go offline
     *  or do other predefined things (none other yet)
     */
    void eventTick(Network* network);

    /**
     *  Checks if any nodes have gone offline based on the timers generated during
     *    the event tick (with a statistical model).  If they go offline, it cancels
     *    the timer and tells the network to send the node offline.  If it comes
     *    back online (another timer), it tells the network and removes the timer.
     */
    void checkOffline(Network* network);
    /**
     * Generates timers for a node going offline and coming back online which will
     * be checked once an event tick.  Times based on the Geometric Statistical Distribution
     */
    void sendOffline(Network* network, UUID nodeUUID, clock_unit_t timeToDisconnect, clock_unit_t timeOffline);
};

#endif
