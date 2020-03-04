#include <iostream>
#include "Random.h"

int main(int argc, char** argv) {
    EventGen* eventGenerator = new Random();
    Simulation* simulation = new Simulation();
    Network* network = simulation->getNetwork();

    network->addNode();

    // Create new nodes and add them to the map
    for(int i = 0; i < AMOUNT_OF_NODES; i++) {
        // FIXME: add appropriate keyspace
        network->addNode();
    }

    for(int i = 0; i < 10; i++) {
        eventGenerator->eventTick(network);
    }

    network->printUUIDList();
    network->printChannels();
}