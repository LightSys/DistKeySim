#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "EventGen.h"
#include "UUID.h"
#include "Network.h"
#include "config.hpp"

class Simulation {
private:
    Network network;
    u_int numNodes;
public:
    Simulation(const struct Config &config);
    ~Simulation() = default;

    // Executes simulation
    void run();

    // Getters
    Network *getNetwork() { return &this->network; }
};

#endif //LIGHTSYS_ADAK_SIMULATION_H
