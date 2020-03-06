#include <iostream>
#include <ctime>

#include "Simulation.h"
#include "EventGen.h"
#include "Random.h"

using namespace std;

Simulation::Simulation(const struct Config &config)
    : numNodes(config.numNodes), network(Network(config.connectionMode)) {
    // Seed random number
    srand(time(nullptr));
}

void Simulation::run() {
    // Create root node that will have the max keyspace 0/0
    network.addRootNode();

    // Create new nodes and add them to the map
    for (int i = 0; i < numNodes; i++) {
        network.addEmptyNode();
        network.checkAndSendAllNodes();
    }
    
    // Loop for EventTicks
//    Random eventGen;
//    for (int i = 0; i < 10; i++) {
//        eventGen.eventTick(&network);
//        network.checkAndSendAllNodes();
//    }

    network.printChannels();
    network.printKeyspaces();
}
