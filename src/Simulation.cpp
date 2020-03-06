#include <iostream>
#include <ctime>

#include "Simulation.h"

using namespace std;

// Number of rounds to complete to allow the simulation to settle
static const int NUM_ROUNDS = 50;

Simulation::Simulation(const struct Config &config)
    : numNodes(config.numNodes), network(Network(config.connectionMode)) {
    // Seed random number
    srand(time(nullptr));
}

void Simulation::run() {
    // Create root node that will have the max keyspace 0/0
    network.addRootNode();
    
    cout << "Root UUID: " << network.getNodes().begin()->first << endl;

    // Create new nodes and add them to the map
    for (int i = 0; i < numNodes; i++) {
        UUID newNodeID = network.addEmptyNode();
        network.checkAndSendAllNodes();
        network.doAllHeartbeat();
    }
    
    for (int &&i = 0; i < NUM_ROUNDS; i++) {
        network.checkAndSendAllNodes();
        network.doAllHeartbeat();
    }

    network.printChannels();
    network.printKeyspaces();
}
