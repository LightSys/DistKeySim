#include <iostream>

#include "Network.h"
#include "Node.h"
#include "UUID.h"

using namespace std;

Network::Network(ConnectionType connectionType) {
    this->connectionType = connectionType;
    cout << "Network online" << endl;
}

bool Network::sendMsg(const Message &message) {
    shared_ptr<Node> destNode = getNodeFromUUID(message.destnodeid());
    return destNode->receiveMessage(message);
}

void Network::doAllHeartbeat() {
    // NOTE: Baylor will need to change this, right now we are sending heartbeat messages practically all the time
    // they will probably want to add time to Node to send it periodically
    for (auto const& node : nodes) {
        node.second->heartbeat();
    }
    Logger::getTimeslot(true);
    Logger::getShared(true,0);
}

void Network::checkAndSendAllNodes() {
    for (auto const& node : nodes) {
        // Check each node for queued messages, moving all messages to here before working
        deque<Message> nodeMsgs = node.second->getMessages();
        bool gaveKeyspace = false;
        for (auto const &msg : nodeMsgs) {
            if (gaveKeyspace && (msg.messagetype() == Message_MessageType_KEYSPACE)) {
                // Gave keyspace, don't give any more
                continue;
            }
            
            gaveKeyspace |= sendMsg(msg);
        }
    }
}

shared_ptr<Node> Network::getNodeFromUUID(const UUID &uuid) const {
    return nodes.find(uuid)->second;
}

UUID Network::addRootNode() {
    return addNode(Keyspace(0, UINT_MAX, 0));
}

UUID Network::addEmptyNode() {
    // Create a new new node with the no keyspace
    auto newNode = make_shared<Node>();
    UUID newUUID = newNode->getUUID();
    
    // Add the new node to the nodes map
    nodes.insert({newNode->getUUID(), newNode});
    
    // Make connection to peers
    connectNodeToNetwork(newNode);
    
    return newUUID;
}

UUID Network::addNode(const Keyspace &keyspace) {
    // Create a new new node with the given keyspace
    auto newNode = make_shared<Node>(keyspace);
    UUID newUUID = newNode->getUUID();
    
    // Make connection to peers
    connectNodeToNetwork(newNode);
    
    // Add the new node to the nodes map
    nodes.insert({newNode->getUUID(), move(newNode)});

    //log node created    
    return newUUID;
}

void Network::connectNodeToNetwork(shared_ptr<Node> newNode) {
    if (this->connectionType == ConnectionType::Full) {
        fullyConnect(newNode);
    } else if (this->connectionType == ConnectionType::Partial) {
        // rand() % 4 is an arbitrary number to make the connection only happen sometimes.
        int coinFlip = rand() % 8;
        for (auto &[uuid, node] : nodes) {
            // Don't connect the node to itself
            if (newNode->getUUID() == node->getUUID()) {
                continue;
            }
            
            // Make sure that the channel doesn't already exist
            if (!channelExists(node->getUUID(), node->getUUID()) && coinFlip == 0) {
                connectNodes(node->getUUID(), node->getUUID());
            }
        }
        
        singleConnect(newNode);
    } else if (this->connectionType == ConnectionType::Single) {
        singleConnect(newNode);
    }
}

void Network::fullyConnect(shared_ptr<Node> node) {
    for (auto const &[uuid, peer] : nodes) {
        // Don't connect the node to itself
        if (node->getUUID() == uuid) {
            continue;
        }
        
        // Make sure that the channel doesn't already exist
        if (channelExists(node->getUUID(), uuid)) {
            continue;
        }
        
        // No connection, create one
        connectNodes(node->getUUID(), uuid);
    }
}

void Network::singleConnect(shared_ptr<Node> node) {
    // Go through each node and make sure it has at least 1 connection
    if (nodes.size() > 1) {
        UUID randomUUID = getRandomNode();

        // On the off chance that getRandomNode() picks the current node, then we need another node
        while (randomUUID == node->getUUID()) {
            randomUUID = getRandomNode();
        }
        connectNodes(node->getUUID(), randomUUID);
    }
}

bool Network::channelExists(const UUID &nodeOne, const UUID &nodeTwo) {
    return getChannelIndex(nodeOne, nodeTwo) != -1;
}

int Network::getChannelIndex(const UUID &nodeOne, const UUID &nodeTwo) {
    auto channelItr = find_if(
        channels.begin(),
        channels.end(),
        [nodeOne, nodeTwo](Channel &channel) {
            return (
                (channel.getToNode() == nodeOne && channel.getFromNode() == nodeTwo) ||
                (channel.getToNode() == nodeTwo && channel.getFromNode() == nodeOne)
            );
        }
    );
    
    return (channelItr != channels.end()) ? distance(channels.begin(), channelItr) : -1;
}

void Network::connectNodes(const UUID &nodeOne, const UUID &nodeTwo) {
    // If trying to connect the same node or trying to create duplicate connection
    if (nodeOne == nodeTwo || channelExists(nodeOne, nodeTwo)) {
        return;
    }
    
    Channel channel(nodeOne, nodeTwo);
    channels.push_back(channel);

    shared_ptr<Node> node1 = getNodeFromUUID(nodeOne);
    shared_ptr<Node> node2 = getNodeFromUUID(nodeTwo);
    node1->addPeer(node2);
    node2->addPeer(node1);
}

void Network::disconnectNodes(const UUID &nodeOne, const UUID &nodeTwo) {
    int channelIndex = getChannelIndex(nodeOne, nodeTwo);
    if (channelIndex > -1) {
        const Channel &channel = channels.at(channelIndex);
        
        // Remove peer from node
        shared_ptr<Node> node1 = getNodeFromUUID(channel.getFromNode());
        shared_ptr<Node> node2 = getNodeFromUUID(channel.getToNode());
        node1->removePeer(node2->getUUID());
        node2->removePeer(node1->getUUID());
        
        channels.erase(channels.begin() + channelIndex);
    }
}

vector<UUID> Network::generateUUIDList() {
    vector<UUID> uuidList;

    // Loop through array and get all the UUIDs
    for (auto const& x : nodes) {
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
    printUUIDList(cout, ' ');
}

void Network::printUUIDList(ostream &out, char spacer) {
    int counter = 0;
    out << "UUID List" << '\n'
        << "COUNT" << spacer << "UUID (in hex)" << spacer << "# bits" << '\n';
    for (UUID const uuid : generateUUIDList()) {
        out << counter << spacer << uuid << spacer << (uuid.size() * 8) << spacer << '\n';
        counter++;
    }
    out << flush;
}
void Network::printChannels() {
    printChannels(cout, ' ');
}

void Network::printChannels(ostream &out, char spacer) {
    out << "CHANNELS\n"
        << "TO" << spacer << "FROM" << spacer << "ID" << "\n";
    for (const Channel &channel : channels) {
        out << channel.getToNode() << spacer
            << channel.getFromNode() << spacer
            << channel.getChannelId()
            << endl;
    }
    out << flush;
}

void Network::printKeyspaces() {
    printKeyspaces(cout, ' ');
}
void Network::printKeyspaces(ostream &out, char spacer) {
    out << "KEYSPACES\n";
    
    int counter = 0;
    for (auto const& x : nodes) {
        for (const Keyspace &keyspace : x.second->getKeySpace()) {
            out << "Node" << spacer << "UUID" << spacer << "Keyspace" << "Start" << spacer << "End" << spacer
                << "Suffix Bits\n"
                << counter << spacer << x.second->getUUID() << spacer
                << keyspace.getStart() << spacer << keyspace.getEnd() << spacer
                << keyspace.getSuffix()
                << endl;
        }
        counter++;
    }
    out << flush;
}
