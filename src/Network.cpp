#include <iostream>

#include "Network.h"
#include "Node.h"
#include "UUID.h"

using namespace std;

Network::Network(ConnectionType connectionType) {
    this->connectionType = connectionType;
    cout << "Network Constructor called" << endl;
}
Network::~Network() {
    // Delete all the channels
    for (Channel* channel : this->channels) {
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
    // Create a new new node with the given keyspace
    Node* node = new Node(keyspace);

    // Add the new node to the map <UUID, Node*>
    this->nodes.emplace(node->getUUID(), node);

    if(this->connectionType == ConnectionType::Full) {
        fullyConnect(node);
    } else if(this->connectionType == ConnectionType::Partial) {
        cout << "STUB: addNode() Partial ConnectionType" << endl;
    } else if(this->connectionType == ConnectionType::Circular) {
        cout << "STUB: addNode() Circlular ConnectionType" << endl;
    } else if(this->connectionType == ConnectionType::Single) {
        cout << "STUB: addNode() Single ConnectionType" << endl;
    }

}

void Network::fullyConnect(Node* node) {
    for(auto nodeWrapper : nodes) {
        Node* nodeListNode = nodeWrapper.second;

        // Don't connect the node to itself
        if(node->getUUID() != nodeListNode->getUUID()) {
            // Make sure that the channel doesn't already exist
            if(!channelExists(node->getUUID(), nodeListNode->getUUID())) {
                connectNodes(node->getUUID(), nodeListNode->getUUID());
            }
        }
    }
}

bool Network::channelExists(UUID nodeOne, UUID nodeTwo) {
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

vector<UUID> Network::generateUUIDList() {
    vector<UUID> uuidList;

    // Loop through array and get all the UUIDs
    for(auto const& x : nodes) {
        uuidList.push_back((UUID) x.first);
    }
    return uuidList;
}

UUID Network::getRandomNode() {
    vector<UUID> uuidList = generateUUIDList();

    // Select a random value from the new UUID list
    int randomNum = rand() % uuidList.size();
    return uuidList.at(randomNum);
}

void Network::sendMsg(Message message) {

}

void Network::printUUIDList() {
    int counter = 0;
    cout << "COUNT - UUID (in hex) - # bits" << endl;
    for(UUID uuid : generateUUIDList()) {
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


