#ifndef LIGHTSYS_ADAK_EVENT_GEN_H
#define LIGHTSYS_ADAK_EVENT_GEN_H

#include "Simulation.h";
class EventGen {
public:
    EventGen() {};

    /**
     * Probably should change to a new name, but this method will do a random action based on the implementation of this
     * pure virtual function.
     * @param simulation
     */
    virtual void doSomethingRandom(Simulation* simulation) = 0;
};

#endif //LIGHTSYS_ADAK_EVENT_GEN_H
