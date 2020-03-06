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

    // Send that message to ALL nodes, even ones the sourceNode doesnt' know about
//    for(auto const& x : nodes) {
//        x.second->receiveMessage(message);
//    }

    // Send the messsage to that Node's peers
    for(Node* node : sourceNode->getPeers()) {
        /// FIXME: this is a temporary fix, without this break when a node asks for keyspace, every single node will give it
        /// this is obviously not the desired functionality. Ideally, we would have each node on it's own thread and it
        /// would loop through (starting with the peers it thinks it will give keyspace. And one at a time send the heartbeat
        /// to them and wait to see if they respond
        if(node->receiveMessage(message)) {
            break;
        }
    }
}

void Network::checkAllNodesForMessages() {
    for(auto const& x : nodes) {
        // TODO: Baylor will need to change this, right now we are sending heartbeat messages practically all the time
        // they will probably want to add time to Node to send it periodically
        sendMsg(x.second->getHeartbeatMessage());
    }
}

UUID Network::addNode() {
    return addNode(new Keyspace(0, UINT_MAX, 0));
}
UUID Network::addNode(Keyspace* keyspace) {
    // Create a new new node with the given keyspace
    Node* node = new Node(keyspace);

    // Add the new node to the map <UUID, Node*>
    this->nodes.emplace(node->getUUID(), node);

    if(this->connectionType == ConnectionType::Full) {
        fullyConnect(node);
    } else if(this->connectionType == ConnectionType::Partial) {
        // rand() % 4 is an arbitrary number to make the connection only happen sometimes.
        int coinFlip = rand() % 8;
        for(auto const nodeWrapper : nodes) {
            Node* nodeListNode = nodeWrapper.second;

            // Don't connect the node to itself
            if(node->getUUID() != nodeListNode->getUUID()) {
                // Make sure that the channel doesn't already exist
                if(!channelExists(node->getUUID(), nodeListNode->getUUID())) {
                    if(coinFlip == 0) {
                        connectNodes(node->getUUID(), nodeListNode->getUUID());
                    }
                }
            }
        }
        singleConnect(node);
    } else if(this->connectionType == ConnectionType::Single) {
        singleConnect(node);
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

void Network::singleConnect(Node* node) {
    // Go through each node and make sure it has at least 1 connection
    if(nodes.size() > 1) {
        UUID randomUUID = getRandomNode();

        // On the off chance that getRandomNode() picks the current node, then we need another node
        while (randomUUID == node->getUUID()) {
            randomUUID = getRandomNode();
        }
        connectNodes(node->getUUID(), randomUUID);
    }
}

bool Network::channelExists(const UUID nodeOne, const UUID nodeTwo) {
    return getChannelIndex(nodeOne, nodeTwo) != -1;
}
int Network::getChannelIndex(const UUID nodeOne, const UUID nodeTwo) {
    for(int i = 0; i < channels.size(); i++) {
        Channel* channel = channels.at(i);
        if(channel->getToNode() == nodeOne || channel->getToNode() == nodeTwo) {
            if(channel->getFromNode() == nodeOne || channel->getFromNode() == nodeTwo) {
                return i;
            }
        }
    }
    return -1;
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

void Network::disconnectNodes(const UUID nodeOne, const UUID nodeTwo) {
    int channelIndex = getChannelIndex(nodeOne, nodeTwo);
    if(channelIndex > -1) {
        Channel* channel = channels.at(channelIndex);
        Node* node1 = this->getNodeFromUUID(channel->getFromNode());
        Node* node2 = this->getNodeFromUUID(channel->getToNode());


        // TODO: need to remove each peer from the Node's peer list
//        node1->remove


        // TODO: haven't checked this
        channels.erase(channels.begin() + channelIndex);
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
        *out << counter << spacer << uuid << spacer << (uuid.size() * 8) << spacer << NewLine;
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
        *out << channel->getToNode() << spacer;
        *out << channel->getFromNode() << spacer;
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
            *out << counter << spacer << x.second->getUUID() << spacer;
            *out << keyspace->getStart() << spacer << keyspace->getEnd() << spacer;
            *out << keyspace->getSuffix() << NewLine;
        }
        counter++;
    }
    *out << flush;
}