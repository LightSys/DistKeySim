#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include <iostream>
#include <ctime>


#include "EventGen.h"
#include "UUID.h"
#include "Network.h"
#include "config.hpp"
#include "Logger.h"
#include "UUID.h"
#include "GeometricDisconnect.h"
#include "Random.h"
#include "SystemClock.h"


using namespace std;

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
