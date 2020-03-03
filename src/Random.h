#ifndef LIGHTSYS_ADAK_RANDOM_H
#define LIGHTSYS_ADAK_RANDOM_H

#include "EventGen.h"
#include "Simulation.h"

class Random: public EventGen {
private:

public:
    Random();
    void doSomethingRandom(Simulation* simulation);
};

#endif //LIGHTSYS_ADAK_RANDOM_H
