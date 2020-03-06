#include <vector>

#include "catch.hpp"
#include "Node.h"
#include "Simulation.h"

using namespace std;

TEST_CASE("Multiple nodes have unique UUIDs", "[main]") {
    // Test that 10 unique Nodes will not have matching UUIDs
    int numNodes = 10;
    vector<Node> testNodes;
    
    for (int &&i = 0; i < numNodes; i++) {
        testNodes.emplace_back(Node());
    }
    
    for (int &&i = 0; i < numNodes; i++) {
        for (int &&j = i + 1; j < numNodes; j++) {
            REQUIRE(testNodes.at(i).getUUID() != testNodes.at(j).getUUID());
        }
    }
}

TEST_CASE("Single Node has minimum index of -1", "[main]") {
    Node testNode;
    REQUIRE(testNode.minimumKeyspaceIndex() == -1);
}

TEST_CASE("Test if keyspace is used", "[main]") {

    auto *simulation = new Simulation(EventGenerationType::Random);
    Network* network = simulation->getNetwork();

    // Create root node that will have the max keyspace 0/0
    network->addNode();

    // Create new nodes and add them to the map
    for(int i = 0; i < 10; i++) {
        network->addNode(nullptr);
        network->checkAllNodesForMessages(); // send heartbeat messages so nodes send keyspace
    }

    // Use every single key and count how many keys it has used (takes a little while)
    adak_key keysUsed = 0;
    for(auto const& x : network->getNodes()) {
        for(Keyspace* keyspace : x.second->getKeySpace()) {
            while(keyspace->isKeyAvailable()) {
                keyspace->getNextAvailableKey();
                keysUsed++;
            }
        }
    }

    cout << "KEYS USED: " << keysUsed << endl;

    // Check each keyspace to make sure that start <= end
    for(auto const& x : network->getNodes()) {
        for(Keyspace* keyspace : x.second->getKeySpace()) {
            REQUIRE(keyspace->getStart() <= keyspace->getEnd() + pow(2, keyspace->getSuffix()));
        }
    }

//    this->network.printUUIDList();
//    this->network.printChannels();
    network->printKeyspaces();
//    this->network.printUUIDList(this->getCSVOutput());
//    this->network.printChannels(this->getCSVOutput());
//    this->network.printKeyspaces(this->getCSVOutput());

    delete simulation;
}
