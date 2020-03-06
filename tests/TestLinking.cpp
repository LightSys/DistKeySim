#include "catch.hpp"
#include "Simulation.h"
#include "Random.h"


TEST_CASE("Testing nodeLinking", "[anything]") {
    EventGen* eventGenerator = new Random();
    Simulation* simulation = new Simulation();
    Network* network = simulation->getNetwork();

    network->addNode();

    // Create new nodes and add them to the map
    for(int i = 0; i < AMOUNT_OF_NODES; i++) {
    network->addNode();
    }
    UUID randomUUID = network->getRandomNode();
    Node* randomNode = network->getNodeFromUUID(randomUUID);
    int randomSize = randomNode->getKeySpace().size();

    for(auto const& x : network->getNodes()) {
        REQUIRE(x.second->getPeers().size() == randomSize);
    }

    delete simulation;
    delete network;
}