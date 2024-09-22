#pragma once

#include <iostream>
#include <ctime>

#include "AbstractStrategy.h"
#include "config.hpp"
#include "Network.h"

using namespace std;

class Simulation {

public:
    Simulation(const struct Config &config);
    ~Simulation() = default;

    // Executes simulation
    void run();

    // Getters
    Network *getNetwork() { return &this->network; }

private:
    Network network;
    u_int numNodes;
    AbstractStrategy *adakStrategy;
};