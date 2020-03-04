#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "UUID.h"
#include "Network.h"

// TODO: change to JSON parameter
static const int AMOUNT_OF_NODES = 2;

class Simulation {
private:
    Network network;
    std::ofstream* csvOutput;
public:
    Simulation();
    ~Simulation();

    // Getters
    Network *getNetwork() { return &this->network; }
    std::ofstream* getCSVOutput() const { return this->csvOutput; }
};

#endif //LIGHTSYS_ADAK_SIMULATION_H