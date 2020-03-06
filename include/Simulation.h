#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "UUID.h"
#include "Network.h"

// TODO: change to JSON parameter
static const int AMOUNT_OF_NODES = 10;

/**
 * The type of Event Generation type the Simluation should use
 */
enum class EventGenerationType { Random, Unused1, Unused2 };

class Simulation {
private:
    EventGenerationType eventGenerationType;
    Network network;
    std::ofstream* csvOutput;
public:
    Simulation(EventGenerationType eventGenerationType);
    ~Simulation();

    void runSimulation();

    // Getters
    Network *getNetwork() { return &this->network; }
    std::ofstream* getCSVOutput() const { return this->csvOutput; }
};

#endif //LIGHTSYS_ADAK_SIMULATION_H