#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "UUID.h"
#include "Network.h"
#include "config.hpp"


// gets parameters from the json file and puts them into the variables.
auto const config = new Config( std::ifstream("test.json"));
static const int AMOUNT_OF_NODES = config->getNumNodes();
static const std::string CONNECTION_MODE = config->getConnectionMode();
static const std:: string CSV_OUTPUT_PATH = config->getCSVOutputPath();
static const int CREATION_RATE = config->getCreationRate();
static const float NETWORK_SCALE_1 = config->getNetworkScale();

/**
 * The type of Event Generation type the Simulation should use
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