#include "catch.hpp"
#include "Node.h"
#include "Random.h"

TEST_CASE("Testing getActive", "[classic]") {
    Node* a = new Node();
    REQUIRE(a->getActive() == true);
    delete a;
};

TEST_CASE("Testing test", "[classic]") {
    Random eventGenerator;
    Simulation simulation;
//    Network* network = simulation.getNetwork();
//
//    network->addNode();
//
//    // Create new nodes and add them to the map
//    for(int i = 0; i < 10; i++) {
//        // FIXME: add appropriate keyspace
//        network->addNode();
//    }
//
//    for(int i = 0; i < 10; i++) {
//        eventGenerator.eventTick(network);
//    }
//
//    network->printUUIDList();
//    network->printChannels();
};