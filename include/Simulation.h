#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H

#include "UUID.h"
#include "Network.h"

// TODO: change to JSON parameter
static const int AMOUNT_OF_NODES = 10;

class Simulation {
public:
    Simulation();
    ~Simulation() {};

    // Getters
    Network *getNetwork() { return &this->network; }
private:
//    std::vector<UUID> uuidList;
//    std::map<UUID, Node*> nodes;
//    std::vector<Channel*> channels;
    Network network;
};

#endif //LIGHTSYS_ADAK_SIMULATION_H