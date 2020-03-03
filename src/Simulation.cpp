#include <iostream>

#include "Simulation.h"
#include "UUID.h"

using namespace std;

int main(int argc, char **argv){

    std::map<UUID, Node*> nodes;

    // Create new nodes and add them to the map
    for(int i = 0; i < 10; i++) {
        Node* newNode = new Node();
        nodes.emplace(newNode->getUUID(), newNode);
    }

    std::vector<Channel*> channels;
    Network* network = new Network(nodes, channels);
}