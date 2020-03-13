
#include "Simulation.h"

// Number of rounds to complete to allow the simulation to settle
static const int NUM_ROUNDS = 50;

Simulation::Simulation(const struct Config &config)
    : numNodes(config.numNodes), network(Network(config.connectionMode, config.visiblePeers, config.lambda3)) {
    // Seed random number
    srand(time(nullptr));
}

void Simulation::run() {
    string message = "Started Simulation";
    Logger::log(message);

    // Create root node that will have the max keyspace 0/0
    network.addRootNode();

    cout << "Root UUID: " << network.getNodes().begin()->first << endl;

    // Create new nodes and add them to the map
    for (int i = 0; i < numNodes; i++) {
        UUID newNodeID = network.addEmptyNode();
        network.checkAndSendAllNodes();
        network.doAllHeartbeat();
    }

    // for (int &&i = 0; i < NUM_ROUNDS; i++) {
    //     network.checkAndSendAllNodes();
    //     network.doAllHeartbeat();
    // }

    double lambda1 = 2.0, lambda2 = 3.0;//time till offline and time till online
    EventGen* eventGen = new GeometricDisconnect(SIMULATION, lambda1, lambda2);
    network.checkAndSendAllNodes();
    network.doAllHeartbeat();
    //send some nodes offline////TODO:: RUN THIS TEST
    network.printChannels();
    eventGen->eventTick(&network);//randomly sends up to 10 nodes offline
    network.printChannels();

    cout << "Ticking network a bunch" << endl;
    for(int i=0; i<NUM_ROUNDS; i++){
        cout << "***********************************************Tick" << endl;
        network.printChannels();
    }

    // Example of an eventTick usage, this is where you would add in the different implementations of EventGen
    // currently only Random() is implemented
    // EventGen* eventGen = new Random();
    // for(int i = 0; i < 10; i++ ) {
    //     eventGen->eventTick(&network);
    // }

    shared_ptr<Node> tomTest = this->network.getNodeFromUUID(this->network.getRandomNode());
    shared_ptr<NodeData> Nodedata = tomTest->getNodeData();
    vector<Keyspace> tomSpace = tomTest->getKeySpace();
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

    // Output to CSV
    ofstream csv;
    csv.open("channelsOut.csv", ofstream::out | ofstream::trunc);
    network.printChannels(csv, ',');
    csv.close();

    csv.open("keyspacesOut.csv", ofstream::out | ofstream::trunc);
    network.printKeyspaces(csv, ',');
    csv.close();

    csv.open("uuidsOut.csv", ofstream::out | ofstream::trunc);
    network.printUUIDList(csv, ',');
    csv.close();

    delete eventGen;
}
