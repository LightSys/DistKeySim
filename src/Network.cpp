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

void Network::sendMsg(const Message message) {
    const UUID sourceUUID = message.sourcenodeid();
    Node* sourceNode = getNodeFromUUID(sourceUUID);
    sourceNode->setMessageWaiting(false);
    for(auto const& x : nodes) {
        x.second->receiveMessage(message);
    }
}

void Network::checkAllNodesForMessages() {
    for(auto const& x : nodes) {
        if(x.second->isMessageWaiting()) {
            sendMsg(x.second->getWaitingMessage());
        }
    }
}

UUID Network::addNode() {
    return addNode(new Keyspace(0, INT32_MAX, 0));
}
UUID Network::addNode(Keyspace* keyspace) {
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
    return node->getUUID();
}

void Network::fullyConnect(Node* node) {
    for(auto const nodeWrapper : nodes) {
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

// FIXME: this doesn't seem to be working, with two nodes there are two channels between them
bool Network::channelExists(const UUID nodeOne, const UUID nodeTwo) {
    for(Channel* channel : channels) {
        if(channel->getToNode() == nodeOne || channel->getToNode() == nodeTwo) {
            if(channel->getFromNode() == nodeOne || channel->getFromNode() == nodeTwo) {
                return true;
            }
        }
    }
    return false;
}

void Network::connectNodes(const UUID nodeOne, const UUID nodeTwo) {
    // If trying to connect the same node, then don't do anything.
    if(nodeOne == nodeTwo) {
        return;
    }
    if(!channelExists(nodeOne, nodeTwo)) {
        auto* channel = new Channel(nodeOne, nodeTwo);
        this->channels.push_back(channel);

        Node *node1 = getNodeFromUUID(nodeOne);
        Node *node2 = getNodeFromUUID(nodeTwo);
        node1->addPeer(node2);
        node2->addPeer(node1);
    }
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

void Network::printUUIDList() {
    printUUIDList(&std::cout, ' ');
}
void Network::printUUIDList(ostream* out, char spacer) {
    static const char NewLine = '\n';
    int counter = 0;
    *out << "UUID List" << NewLine;
    *out << "COUNT" << spacer << "UUID (in hex)" << spacer << "# bits" << NewLine;
    for(UUID const uuid : generateUUIDList()) {
        *out << counter << spacer << UUIDToHex(uuid) << spacer << (uuid.size() * 8) << spacer << NewLine;
        counter++;
    }
    *out << flush;
}
void Network::printChannels() {
    printChannels(&std::cout, ' ');
}
void Network::printChannels(ostream* out, char spacer) {
    static const char NewLine = '\n';
    *out << "CHANNELS" << NewLine;
    *out << "TO" << spacer << "FROM" << spacer << "ID" << NewLine;
    for(Channel* channel : this->channels) {
        *out << UUIDToHex(channel->getToNode()) << spacer;
        *out << UUIDToHex(channel->getFromNode()) << spacer;
        *out << channel->getChannelId() << NewLine;
    }
    *out << flush;
}

void Network::printKeyspaces() {
    printKeyspaces(&std::cout, ' ');
}
void Network::printKeyspaces(std::ostream *out, char spacer) {
    static const char NewLine = '\n';
    *out << "KEYSPACES" << NewLine;
    int counter = 0;
    for(auto const& x : nodes) {
        for(Keyspace* keyspace : x.second->getKeySpace()) {
            *out << "Node" << spacer << "UUID" << spacer << "Keyspace" << "Start" << spacer << "End" << spacer;
            *out << "Suffix Bits" << NewLine;
            *out << counter << spacer << UUIDToHex(x.second->getUUID()) << spacer;
            *out << keyspace->getStart() << spacer << keyspace->getEnd() << spacer;
            *out << keyspace->getSuffix() << NewLine;
        }
        counter++;
    }
    *out << flush;
}