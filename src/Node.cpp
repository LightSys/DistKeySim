

#include "Node.h"

using namespace std;

Node::Node(double lambda3) : uuid(new_uuid()), lastDay(NodeData()), lambda3(lambda3)  {
    generateObjectCreationRateDistribution();
    changeConsumptionRate();
}

Node::Node(const Keyspace &keySpace, double lambda3) : uuid(new_uuid()), lastDay(NodeData()), lambda3(lambda3) {
    keyspaces.push_back(keySpace);
    generateObjectCreationRateDistribution();
    changeConsumptionRate();
}

void Node::consumeObjects(){
    amountOfOneKeyUsed += objectConsuptionRatePerSecond;
    if(amountOfOneKeyUsed >= 1.0){
        this->getNextKey();
        amountOfOneKeyUsed--;
    }
}

void Node::generateObjectCreationRateDistribution(){
    d3 = new geometric_distribution<>(this->lambda3);
    // These are required for the geometric distribution function to operate
    // correctly
    random_device rd;
    // Save the seed for debugging
    unsigned int seed = rd();
    gen = new mt19937(seed);
}

#include <iostream>
void Node::changeConsumptionRate(){
    double randomNum = (*d3)(*gen) + 1;
    objectConsuptionRatePerSecond = 1.0/(randomNum);
    std::cout << "Changed object consumption rate to " << objectConsuptionRatePerSecond << std::endl;
}

static Node rootNode(double lambda3) {
    return Node(Keyspace(0, UINT_MAX, 0), lambda3);
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

shared_ptr<NodeData> Node::getNodeData() const {
    return make_shared<NodeData>(lastDay);
}

ADAK_Key_t Node::getNextKey() {
    lastDay.useKey();
    int index = minimumKeyspaceIndex();
    if (index == -1){

        ///Baylor This does mean there is an error in the code its more in how to distibute Keys correctly.
        ///Usually this happens in long term allocation statistics because it goes through till the end of
        ///the keyspace to see if th allocation is an issue. However if this happens. The problem is that the keys are
        ///getting distributed more than the current node has capcity for.

        string message = "ERROR from getNextKey in Node: Not more keys to give";
        cout << message <<endl;
        Logger::log(message);
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

bool Node::isKeyAvailable(){
    bool isAKey = false;
    for(const Keyspace& keys: this->keyspaces ){
        if(keys.isKeyAvailable()){
            isAKey = true;
            break;
        }
    }
    return isAKey;
}

/**
 * This is where the Baylor team will add more statistics to handle the messages that each node receives
 * @param message
 */
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
    logInfoForHeartbeat();
}

bool Node::receiveMessage(const Message &msg) {
    if (msg.sourcenodeid() == uuid) {
        // Destination node is this node, don't receive it
        return false;
    }

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
    auto peer = peers.find(msg.sourcenodeid());
    if (peer == peers.end()) {
        // First message received from this peer, add message ID and update find result
        peers.insert({msg.sourcenodeid(), make_shared<Message>(msg)});
        peer = peers.find(msg.sourcenodeid());
    } else {
        // Known peer, update msg ID
        peers.at(msg.sourcenodeid()) = make_shared<Message>(msg);
    }

    switch (msg.messagetype()) {
        case Message_MessageType_KEYSPACE:
            // Receiving keyspace from a peer, generate new one for local store
            for (auto &&i = 0; i < msg.keyspace().keyspaces_size(); i++) {
                KeyspaceMessageContents::Keyspace peerSpace = msg.keyspace().keyspaces(i);
                Keyspace newSpace = Keyspace{peerSpace.startid(), peerSpace.endid(), peerSpace.suffixbits()};
                keyspaces.emplace_back(newSpace);
                totalLocalKeyspaceSize+=newSpace.getSize(); //updating total after recieving
            }

            break;

        case Message_MessageType_INFORMATION:
        {
            double allocationRatio = -1;
            for (int &&i = 0; i < msg.info().records_size(); i++) {
                allocationRatio = msg.info().records(i).creationratedata().shortallocationratio();
            }
            if (allocationRatio > ALLOCATION_BEFORE_GIVING_KEYSPACE) {
                if (peer == peers.end()) {
                    // No peer found matching UUID in message, ignore this message
                    break;
                }

                // TODO: Handle decision on giving of keyspace
                // Peer found, decided to share (congrats, we're not 2 year olds!)
                if (!keyspaces.empty()) {
                    Keyspace keyspaceToGive = keyspaces.at(minimumKeyspaceIndex());

                    /// FIXME: Baylor, this something to consider. Right now if the keyspace is running low, then
                    /// there will be a case when we have the following scenarios below happen. These are NOT currently
                    /// covered. These was mainly fixed when we got the allocation working.
                    /// If we have 1 keyspace, and only 1 spot open in that keyspace
                    /// then do nothing.
                    /// If we have more than 1 keyspace with 1 spot open
                    /// give the entire keyspace

                    // If i have another key spot available
                    if ((keyspaceToGive.getStart() + pow(2, keyspaceToGive.getSuffix())) < keyspaceToGive.getEnd()) {
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

                        return true;
                    }
                }
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

    ADAK_Key_t myStart = minKeyspace.getStart();
    ADAK_Key_t myEnd = minKeyspace.getEnd();
    uint32_t mySuffix = minKeyspace.getSuffix();
    mySuffix += 1;

    ADAK_Key_t newStart = minKeyspace.getStart();
    ADAK_Key_t newEnd = minKeyspace.getEnd();
    uint32_t newSuffix = minKeyspace.getSuffix();
    newStart += pow(2, newSuffix);
    newSuffix += 1;

    KeyspaceExchangeRecord newKeyspace{};

    if (newSuffix < 32) {
        // Update local keyspace records
        keyspaces.at(minKeyspaceIndex) = Keyspace(myStart, myEnd, mySuffix);
        newKeyspace = KeyspaceExchangeRecord{"share", newStart, newEnd, newSuffix};
    } else {
        // Start breaking into sub-blocks!
        // (((end - start) / 2^B) / (chunkiness)) * B + start
        // B is the suffix bits

        ADAK_Key_t myStart2 = minKeyspace.getStart();
        ADAK_Key_t myEnd2 = minKeyspace.getEnd();
        uint32_t suffix = minKeyspace.getSuffix();

        double amountOfBlocks = (double) (myEnd2 - myStart2) / pow(2, suffix);
        amountOfBlocks = amountOfBlocks + 0.5 - (amountOfBlocks < 0);

        ADAK_Key_t amountOfBlocksToGive = ((uint32_t) amountOfBlocks) / CHUNKINESS;
        ADAK_Key_t blocksScaled = amountOfBlocksToGive * suffix;
        ADAK_Key_t myNewEnd = blocksScaled + myStart2;

        newKeyspace = KeyspaceExchangeRecord{"share", myNewEnd, myEnd2, suffix};

        keyspaces.at(minKeyspaceIndex) = Keyspace(myStart2, myNewEnd, mySuffix);
        int oldValue = totalLocalKeyspaceSize;
        totalLocalKeyspaceSize =0;
        //reseting total after gave away
        for(Keyspace k: keyspaces){
            totalLocalKeyspaceSize += k.getSize();
        }
        Logger::getShared(false,oldValue-totalLocalKeyspaceSize);
    }

    // Update message type and contents
    toKeyspaceMessage(msg, {newKeyspace});

    string message = "Keyspace split";
    Logger::log(message);
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
        shared_ptr<Message> lastReceived = peers.at(peerID);
        msg = newBaseMessage(
            uuid,
            peerID,
            lastReceived == nullptr ? 0 : lastReceived->lastreceivedmsg(),
            Message_ChannelState_NORMAL_COMMUNICATION,
            messageID
        );
    }

    // HACK
    /// NOTE, this is basically fixed in the develop branch, it is only here right now to tell other nodes,
    /// "If I don't have a keyspace, than give me a keyspace", this is because 1.0 is above the treshold ALLOCATION_BEFORE_GIVING_KEYSPACE
    double allocation = 0;
    if (keyspaces.empty()) {
        allocation = 1;
    }

    toInformationalMessage(
        msg,
        {
            CollectionInfoRecord{"test", createdDay, createdWeek, allocation, allocation},
        }
    );

    return msg;
}

void Node::logInfoForHeartbeat(){
    string tempValue = "1.0";
    vector<string> dataLine;
    dataLine.push_back(uuid);
    dataLine.push_back(to_string((Logger::getTimeslot(false))));
    ADAK_Key_t totalSize =0;
    for(Keyspace k: keyspaces){
        totalSize += k.getSize();
    }
    dataLine.push_back((to_string(totalSize)));
    dataLine.push_back(tempValue);
    dataLine.push_back(to_string(objectConsuptionRatePerSecond));
    //dataLine.push_back(to_string(Logger::getShared(false,0)));
    //dataLine.push_back(to_string(Logger::getConsumption(false,0)));
    if(getTotalLocalKeyspaceSize() >0){
        ADAK_Key_t localSize = getTotalLocalKeyspaceSize();
        double percent = (double)totalSize/(double)localSize;
        dataLine.push_back(to_string(percent));
    }else{
        dataLine.push_back("0.0");
    }

    Logger::logStats(dataLine);
    //maybe put if ran out of space here, and uuid
}
