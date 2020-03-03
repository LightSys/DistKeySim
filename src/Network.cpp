#include <iostream>

#include "Network.h"
#include "UUID.h"

using namespace std;

Network::Network() {
    cout << "Network Constructor called" << endl;
}
Network::~Network() {
    // Delete all the channels
    for(Channel* channel : this->channels) {
        delete channel;
    }

    // Delete all the nodes
    for(auto const& [key, val] : nodes ) {
        delete val;
    }
}

void Network::addNode() {
    addNode(new Keyspace(0, INT32_MAX, 0));
}
void Network::addNode(Keyspace* keyspace) {
    Node* node = new Node(keyspace);
    // Add the UUID to the UUIDList
    this->uuidList.push_back(node->getUUID());

    // Add the new node to the map <UUID, Node*>
    this->nodes.emplace(node->getUUID(), node);
}


void Network::connectNodes(UUID nodeA, UUID nodeB) {
//    Channel channel(nodeA, nodeB);
    Channel* channel = new Channel(nodeA, nodeB);
    // FIXME: What is channelId?
    // FIXME: What happens if a node is connected to itself?
    this->channels.push_back(channel);
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
    cout << "UUID - Node*" << endl;
    for(auto const& [key, val] : nodes ) {
        cout << key << " - " << val << endl;
    }
}

