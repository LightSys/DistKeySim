#include <iostream>
#include "Random.h"

int main(int argc, char** argv) {
    EventGen* eventGenerator = new Random();
    auto* simulation = new Simulation();
    Network* network = simulation->getNetwork();

    // Create root node that will have the max keyspace 0/0
    network->addNode();

    // Create new nodes and add them to the map
    for(int i = 0; i < AMOUNT_OF_NODES; i++) {

        network->printKeyspaces();
        network->printKeyspaces(simulation->getCSVOutput());

        network->addNode(nullptr);
        network->checkAllNodesForMessages();
    }

    for(int i = 0; i < 2; i++) {
        eventGenerator->eventTick(network);
        network->checkAllNodesForMessages();
    }

//    network->printUUIDList();
//    network->printChannels();
    network->printKeyspaces();
//    network->printUUIDList(simulation->getCSVOutput());
//    network->printChannels(simulation->getCSVOutput());
    network->printKeyspaces(simulation->getCSVOutput());
}