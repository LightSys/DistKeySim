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

    fullyConnect(node);
}

void Network::fullyConnect(Node* node) {
    for(auto nodeWrapper : nodes) {
        Node* nodeListNode = nodeWrapper.second;

        // Don't connect the node to itself
        if(node->getUUID() != nodeListNode->getUUID()) {
            // Make sure that the channel doesn't already exist
            if(!channelAlreadyExists(node->getUUID(), nodeListNode->getUUID())) {
                connectNodes(node->getUUID(), nodeListNode->getUUID());
            }
        }
    }
}

bool Network::channelAlreadyExists(UUID nodeOne, UUID nodeTwo) {
    for(Channel* channel : channels) {
        if(channel->getToNode() == nodeOne || channel->getToNode() == nodeTwo) {
            if(channel->getFromNode() == nodeOne || channel->getFromNode() == nodeTwo) {
                return true;
            }
        }
    }
    return false;
}

void Network::connectNodes(UUID nodeOne, UUID nodeTwo) {
    // If trying to connect the same node, then don't do anything.
    if(nodeOne == nodeTwo) {
        return;
    }
    Channel* channel = new Channel(nodeOne, nodeTwo);

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