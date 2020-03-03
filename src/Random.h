#ifndef LIGHTSYS_ADAK_RANDOM_H
#define LIGHTSYS_ADAK_RANDOM_H

#include "EventGen.h"
#include "Simulation.h"

class Random: public EventGen {
private:

public:
    Random();
    void doSomethingRandom(Network* network);
};

#endif //LIGHTSYS_ADAK_RANDOM_H
