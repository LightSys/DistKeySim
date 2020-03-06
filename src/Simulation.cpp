#include <iostream>
#include <ctime>

#include "Simulation.h"
#include "EventGen.h"
#include "Random.h"

using namespace std;

Simulation::Simulation(const u_int &numNodes, const ConnectionType connType)
    : numNodes(numNodes), network(Network(connType)) {
    // Seed random number
    srand(time(nullptr));
}

void Simulation::run() {
    // Create root node that will have the max keyspace 0/0
    this->network.addNode();

    // Create new nodes and add them to the map
    for (int i = 0; i < numNodes; i++) {
        this->network.printKeyspaces();
        this->network.addNode(nullptr);
        this->network.checkAllNodesForMessages();
    }
    
    this->network.printKeyspaces();
    this->network.printUUIDList();
    this->network.printChannels();
    this->network.printKeyspaces();
}