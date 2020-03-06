#include <climits>
#include <iostream>

#include "Node.h"
#include "NodeData.h"

using namespace std;

Node::Node() : uuid(new_uuid()), lastDay(NodeData())  {}

Node::Node(const Keyspace &keySpace) : uuid(new_uuid()), lastDay(NodeData()) {
    keyspaces.push_back(keySpace);
}

static Node rootNode() {
    return Node(Keyspace(0, UINT_MAX, 0));
}

void Node::addPeer(const UUID &peerUUID) {
    if (peers.find(peerUUID) != peers.end()) {
        // Found a match, don't add duplicate connections
        return;
    }
    
    peers.insert({peerUUID, nullptr});
}

void Node::addPeer(shared_ptr<Node> peer) {
    if (peers.find(peer->getUUID()) != peers.end()) {
        // Found a match, don't add duplicate connections
        return;
    }
    
    peers.insert({peer->getUUID(), nullptr});
}

void Node::removePeer(shared_ptr<Node> peer) {
    auto foundPeer = peers.find(peer->getUUID());
    if (foundPeer != peers.end()) {
        // Found a match, don't add duplicate connections
        return;
    }
    
    // Found match, remove it
    peers.erase(foundPeer);
}

void Node::removePeer(const UUID &peerUUID) {
    // Search for local peer matching UUID
    auto foundPeer = peers.find(peerUUID);
    if (foundPeer != peers.end()) {
        // No peers that match that UUID exist
        return;
    }
    
    // Found match, remove it
    peers.erase(foundPeer);
}

shared_ptr<const NodeData> Node::getNodeData() const {
    return make_shared<NodeData>(lastDay);
}

ADAK_Key_t Node::getNextKey() {
    lastDay.useKey();
    int index = minimumKeyspaceIndex();
    if (index == -1){
        cout << "ERROR from getNextKey in Node: Not more keys to give";
        return -1;
    } else {
        return this->keyspaces.at(index).getNextAvailableKey();
    }
}

int Node::minimumKeyspaceIndex() {
    unsigned long min = ULONG_MAX;
    int index = -1;
    
    for (int i = 0; i < keyspaces.size(); i++){
        if (keyspaces.at(i).getStart() < min && keyspaces.at(i).isKeyAvailable()) {
            min = keyspaces.at(i).getStart();
            index = i;
        }
    }
    return index;
}

void Node::heartbeat() {
    // No peers connected, so send
    if (peers.empty()) {
        sendQueue.push_back(getHeartbeatMessage(BROADCAST_UUID));
        messageID++;
        return;
    }
    
    for (const auto &[uuid, _] : peers) {
        // Create heartbeat message for each peer
        sendQueue.push_back(getHeartbeatMessage(uuid));
    }
}

bool Node::receiveMessage(const Message &message) {
    // Check time and update lastDay and rotate the history
    if (NodeData::isNewDay(lastDay.getDay())) {
        history.push_back(lastDay);
        
        if (history.size() > 7) {
            // Remove the first value from the vector, shifting the time, so we only store 1 week
            history.erase(history.begin());
        }
        
        lastDay = NodeData();
    }
    
    // Handle last received message ID incrementing
    auto peer = peers.find(message.sourcenodeid());
    if (peer == peers.end()) {
        // First message received from this peer, add message ID and update find result
        peers.insert({message.sourcenodeid(), make_shared<Message>(message)});
        peer = peers.find(message.sourcenodeid());
    } else {
        // Known peer, update message ID
        peers.at(message.sourcenodeid()) = make_shared<Message>(message);
    }

    switch (message.messagetype()) {
        case Message_MessageType_KEYSPACE:
            // Receiving keyspace from a peer, generate new one for local store
            for (auto &&i = 0; i < message.keyspace().keyspaces_size(); i++) {
                KeyspaceMessageContents::Keyspace peerSpace = message.keyspace().keyspaces(i);
                keyspaces.emplace_back(
                    Keyspace{peerSpace.startid(), peerSpace.endid(), peerSpace.suffixbits()}
                );
            }
            
            break;
    
        case Message_MessageType_INFORMATION:
        {
            double allocationRatio = -1;
            for (int &&i = 0; i < message.info().records_size(); i++) {
                allocationRatio = message.info().records(i).creationratedata().shortallocationratio();
            }
            if (allocationRatio > ALLOCATION_BEFORE_GIVING_KEYSPACE) {
                if (peer == peers.end()) {
                    // No peer found matching UUID in message, ignore this message
                    break;
                }
                
                // TODO: Handle decision on giving of keyspace
                // Peer found, decided to share (congrats, we're not 2 year olds!)
                Message shareSpaceMsg = newBaseMessage(
                    uuid,
                    peer->first,
                    peer->second->lastreceivedmsg(),
                    Message_ChannelState_NORMAL_COMMUNICATION,
                    messageID++
                );
                shareKeyspace(shareSpaceMsg);
                
                // Send message to peer
                sendQueue.push_back(shareSpaceMsg);
            }
        }
        break;
        
        default:
            // Should never reach this point in normal operation
            throw invalid_argument("message invalid type");
    }
    
    return false;
}

deque<Message> Node::getMessages() {
    deque<Message> toSend = move(sendQueue);
    sendQueue.empty();
    return toSend;
}

void Node::shareKeyspace(Message &msg) {
    int minKeyspaceIndex = minimumKeyspaceIndex();

    Keyspace minKeyspace = keyspaces.at(minKeyspaceIndex);

    uint32_t myStart = minKeyspace.getStart();
    uint32_t myEnd = minKeyspace.getEnd();
    uint32_t mySuffix = minKeyspace.getSuffix();
    mySuffix += 1;

    uint32_t newStart = minKeyspace.getStart();
    uint32_t newEnd = minKeyspace.getEnd();
    uint32_t newSuffix = minKeyspace.getSuffix();
    newStart += pow(2, newSuffix);
    newSuffix += 1;
    
    // Update local keyspace records
    keyspaces.at(minKeyspaceIndex) = Keyspace(myStart, myEnd, mySuffix);
    
    // Update message type and contents
    toKeyspaceMessage(msg, {KeyspaceExchangeRecord{"share", newStart, newEnd, newSuffix}});
}

Message Node::getHeartbeatMessage(const UUID &peerID) const {
    Message msg;
    if (peerID == BROADCAST_UUID) {
        // Broadcasting
        msg = newBaseMessage(
            uuid,
            peerID,
            0,
            Message_ChannelState_INITIAL_STARTUP,
            messageID
        );
    } else {
        msg = newBaseMessage(
            uuid,
            peerID,
            peers.at(peerID)->lastreceivedmsg(),
            Message_ChannelState_NORMAL_COMMUNICATION,
            messageID
        );
    }
    
    toInformationalMessage(
        msg,
        {
            CollectionInfoRecord{"test", createdDay, createdWeek, 1.0, 1.0},
        }
    );
    
    return msg;
}