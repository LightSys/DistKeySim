#include <iostream>
#include "Network.h"

using namespace std;
Network::Network(std::map<std::string, Node*> nodes, std::vector<Channel*> channels) : nodes(nodes), channels(channels) {
    cout << "Network Constructor called" << endl;

    for(auto nodeMap : nodes) {
        Node* node = (Node*) nodeMap.second;
        node->printUUID();
    }
}

