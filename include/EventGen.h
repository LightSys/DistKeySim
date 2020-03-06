#ifndef LIGHTSYS_ADAK_EVENT_GEN_H
#define LIGHTSYS_ADAK_EVENT_GEN_H

#include "Simulation.h"
#include "Network.h"

/**
* The type of Event Generation type the Simluation should use
*/
enum class EventGenerationType { Random, Unused1, Unused2 };

class EventGen {
public:
    EventGen() {};

    /**
     * Probably should change to a new name, but this method will do a random action based on the implementation of this
     * pure virtual function.
     * @param simulation
     */
    virtual void eventTick(Network* network) = 0;
};

#endif //LIGHTSYS_ADAK_EVENT_GEN_H
