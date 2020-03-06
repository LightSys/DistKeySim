#ifndef LIGHTSYS_ADAK_RANDOM_H
#define LIGHTSYS_ADAK_RANDOM_H

/**
 * This is an implementation of EventGen class. It is not currently used or fully implemented, but it is intended
 * to be used as a source of random event generation such as network "disconnects", lost messages, etc.
 */

#include "EventGen.h"

class Random: public EventGen {
private:

public:
    Random();
    void eventTick(Network* network);
};

#endif //LIGHTSYS_ADAK_RANDOM_H
