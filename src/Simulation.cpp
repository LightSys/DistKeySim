#include <iostream>
#include <ctime>

#include "Simulation.h"

using namespace std;

Simulation::Simulation(const struct Config &config) : numNodes(config.numNodes), network(Network(config.connectionMode)) {
    // Seed random number
    srand(time(nullptr));
}

void Simulation::run() {
    // Create root node that will have the max keyspace 0/0
    this->network.addNode();

    //Testing statisics
    Node* tomTest = network.getNodeFromUUID(network.getRandomNode());

    // Create new nodes and add them to the map
    for (int i = 0; i < numNodes; i++) {
        this->network.printKeyspaces();
        this->network.addNode();
        this->network.checkAndSendAllNodes();
    }
    
    network.printKeyspaces();
    network.printUUIDList();
    network.printChannels();
    network.printKeyspaces();
    for (int i = 0; i < AMOUNT_OF_NODES; i++) {
        this->network.addNode(nullptr);
        this->network.checkAllNodesForMessages();
    }

    // Loop for EventTicks
    for (int i = 0; i < 10; i++) {
        eventGenerator->eventTick(&this->network);
        this->network.checkAllNodesForMessages();
    }

    NodeData* Nodedata = tomTest->getLastDay();
    cout << "Creation Rate: " << Nodedata->updateCreationRate() <<
            "\nLong Term Allocation: " << Nodedata->updateLongTermAllocationRatio() <<
            "\nShort Term Allocation: " << Nodedata->updateShortTermAllocationRatio() <<
            "\nEnd Key: " << Nodedata->findEndKey(Nodedata->getCreationRate()) <<
            "\nMin Key: " << Nodedata->getMinKeyIndex(tomTest->getKeySpace()) <<
            "\nSuffix: " << tomTest->getKeySpace().at(0)->getSuffix()<<
            "\nFinal Key in actual test: " << tomTest->getKeySpace().at(0)->getEnd() <<endl;

    network.printUUIDList();
    network.printChannels();
    network.printKeyspaces();
}
