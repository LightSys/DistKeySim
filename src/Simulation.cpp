#include <iostream>

#include "Simulation.h"
#include "UUID.h"
#include "EventGen.h"
#include "Random.h"

using namespace std;

Simulation::Simulation() {
    // Seed random number
    srand(time(NULL));
    this->network = new Network();
}

int main(int argc, char **argv){

    EventGen* eventGenerator = new Random();
    Simulation* simulation = new Simulation();
    Network* network = simulation->getNetwork();

    // Create new nodes and add them to the map
    for(int i = 0; i < AMOUNT_OF_NODES; i++) {
        network->addNode(new Node());
    }

    for(int i = 0; i < 10; i++) {
        eventGenerator->doSomethingRandom(network);
    }

    network->printUUIDList();
    network->printChannels();
}