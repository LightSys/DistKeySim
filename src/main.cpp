#include <iostream>
#include "Random.h"

int main(int argc, char** argv) {
    EventGen* eventGenerator = new Random();
    Simulation* simulation = new Simulation();
    Network* network = simulation->getNetwork();

    UUID node1 = network->addNode();
    UUID node2 = network->addNode(nullptr);

    // FIXME: make sure the keyspace is given
    network->connectNodes(node1, node2);
    network->checkAllNodesForMessages();

//    // Create new nodes and add them to the map
//    for(int i = 0; i < AMOUNT_OF_NODES; i++) {
//        // FIXME: add appropriate keyspace
//        network->addNode();
//    }
//
//    for(int i = 0; i < 10; i++) {
//        eventGenerator->eventTick(network);
//    }

    network->printUUIDList();
    network->printChannels();
}