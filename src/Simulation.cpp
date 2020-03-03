//
// Created by tomlo on 3/2/2020.
//
#include "Simulation.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv){

    std::map<std::string, Node*> nodes;

    // Create new nodes and add them to the map
    for(int i = 0; i < 10; i++) {
        Node* newNode = new Node();
        nodes.emplace(newNode->getUUID(), newNode);
//        nodes.insert(newNode->getNodeID(), newNode);
    }

    std::vector<Channel*> channels;
    Network* network = new Network(nodes, channels);
}