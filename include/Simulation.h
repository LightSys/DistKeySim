#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "UUID.h"
#include "Network.h"
#include "config.hpp"

/**
 * The type of Event Generation type the Simluation should use
 */
enum class EventGenerationType { Random, Unused1, Unused2 };

class Simulation {
private:
    EventGenerationType eventGenerationType;
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
