#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "UUID.h"
#include "Network.h"

class Simulation {
private:
    Network network;
    u_int numNodes;
public:
    Simulation(const u_int &numNodes, const ConnectionType connType = ConnectionType::Full);
    ~Simulation() = default;

    // Executes simulation
    void run();

    // Getters
    Network *getNetwork() { return &this->network; }
};

#endif //LIGHTSYS_ADAK_SIMULATION_H