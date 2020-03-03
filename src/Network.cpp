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
    for(auto const& x : nodes ) {
        delete x.second;
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


void Network::connectNodes(UUID nodeOne, UUID nodeTwo) {
    Channel* channel = new Channel(nodeOne, nodeTwo);
    // FIXME: What happens if a node is connected to itself?
    // FIXME: Channel already exists?
    this->channels.push_back(channel);

    Node* node1 = getNodeFromUUID(nodeOne);
    Node* node2 = getNodeFromUUID(nodeTwo);
    node1->addPeer(node2);
    node2->addPeer(node1);
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