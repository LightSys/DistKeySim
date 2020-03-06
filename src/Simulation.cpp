#include <iostream>
#include <ctime>

#include "Simulation.h"
#include "UUID.h"
#include "EventGen.h"
#include "Random.h"

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
    
    // Loop for EventTicks
//    Random eventGen;
//    for (int i = 0; i < 10; i++) {
//        eventGen.eventTick(&network);
//        network.checkAndSendAllNodes();
//    }

    shared_ptr<Node> tomTest = this->network.getNodeFromUUID(this->network.getRandomNode());
    shared_ptr<NodeData> Nodedata = tomTest->getNodeData();
    vector<Keyspace> tomSpace = tomTest->getKeySpace();
    //Nodedata->setCreationRate(10.0);
    cout << "Creation Rate: " << Nodedata->getKeysUsed() <<
            "\nLong Term Allocation: " << Nodedata->updateLongTermAllocationRatio(tomSpace) <<
            "\nShort Term Allocation: " << Nodedata->updateShortTermAllocationRatio(tomSpace) <<
            "\nEnd Key: " << Nodedata->findEndKey(Nodedata->getCreationRate(), tomSpace) <<
            "\nMin Key: " << tomTest->getKeySpace().at(Nodedata->getMinKeyIndex(tomSpace)).getStart() <<
            "\nSuffix: " << tomTest->getKeySpace().at(0).getSuffix()<<
            "\nFinal Key in actual test: " << tomTest->getKeySpace().at(0).getEnd() <<
            "\nProvisional Ration: " << Nodedata->updateProvisioningRatio(Nodedata->getKeysUsed(), Nodedata->getShortTermAllocationRatio());
    network.printChannels();
    network.printKeyspaces();
}
