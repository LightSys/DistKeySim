#include <iostream>

#include "Network.h"
#include "UUID.h"

using namespace std;
//Network::Network(std::map<UUID, Node*> nodes, std::vector<Channel*> channels) : nodes(nodes), channels(channels) {
//    cout << "Network Constructor called" << endl;
//
//    for(auto nodeMap : nodes) {
//        Node* node = (Node*) nodeMap.second;
//    }
//}

Network::Network() {
    cout << "Network Constructor called" << endl;
}

void Network::addNode(Node* node) {
    // Add the UUID to the UUIDList
    this->uuidList.push_back(node->getUUID());

    // Add the new node to the map <UUID, Node*>
    this->nodes.emplace(node->getUUID(), node);
}

void Network::connectNodes(UUID nodeA, UUID nodeB) {
    Channel channel(nodeA, nodeB);

    // FIXME: What is channelId?
    // FIXME: What happens if a node is connected to itself?
    this->channels.push_back(&channel);
}


UUID Network::getRandomNode() {
    int randomNum = rand() % this->uuidList.size();
    return this->uuidList.at(randomNum);
}

void Network::printUUIDList() {
    int counter = 0;
    cout << "COUNT - UUID (in hex) - # bits" << endl;
    for(UUID uuid : this->getUUIDList()) {
        cout << counter << " - " << UUIDToHex(uuid) << " - " << (uuid.size() * 8) << " bits" << endl;
        counter++;
    }
}

void Network::printChannels() {
    for(Channel* channel : this->channels) {
        cout << "To: " << UUIDToHex(channel->getToNode()) << flush;
        cout << " From: " << UUIDToHex(channel->getFromNode()) << flush;
        cout << " ID: " << channel->getChannelId() << endl;
    }
}

void Network::printNodeList() {
//    cout << "UUID - Node*" << endl;
//    for(auto const& [key, val] : nodes ) {
//        cout << key << " - " << val << endl;
//    }
}

