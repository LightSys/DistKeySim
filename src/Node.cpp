#include <climits>
#include <iostream>

#include "Node.h"
#include "NodeData.h"

using namespace std;

static const HexDigest& BROADCAST_UUID = "00000000-0000-0000-0000000000";

Node::Node() : uuid(new_uuid()) {
    // TODO: figure out how to call Node(Keyspace* keySpace) constructor
    Node(new Keyspace(0, ULONG_MAX, 0));
}

Node::~Node() {
    for(Keyspace* keyspace : keySpace) {
        delete keyspace;
    }
    for(Node* node : peers) {
        delete node;
    }
    for(NodeData* nodeData : history) {
        delete nodeData;
    }
    delete lastDay;
}

Node::Node(Keyspace* keySpace) {
    this->uuid = new_uuid();
    lastDay = new NodeData(this);

    if (keySpace != nullptr) {
        this->keySpace.push_back(keySpace);
    }
}

adak_key Node::getNextKey() {
    lastDay->useKey();
    int index = minimumKeyspaceIndex();
    if (index == -1){
        cout << "ERROR from getNextKey in Node: Not more keys to give";
        return -1;
    } else {
        return this->keySpace.at(index)->getNextAvailableKey();
    }
}

int Node::minimumKeyspaceIndex() {
    unsigned long min = ULONG_MAX;
    int index = -1;
    for(int i = 0; i < keySpace.size(); i++){
        if(keySpace[i]->getStart() < min && keySpace[i]->isKeyAvailable()){
            min = keySpace[i]->getStart();
            index = i;
        }
    }
    return index;
}

/**
 * This is where the Baylor team will add more statistics to handle the messages that each node receives
 * @param message
 */
bool Node::receiveMessage(const Message message) {
    // Check time and update lastDay and rotate the history
    if (NodeData::isNewDay(lastDay->getDay())) {
        history.push_back(lastDay);
        if(history.size() > 7) {
            // Remove the first value from the vector
            // this shifts the time, so we only store 1 week
            delete history.at(0);
            history.erase(history.begin());
        }
        lastDay = new NodeData(this);
    }

    if(message.messagetype() == Message::MessageType::Message_MessageType_KEYSPACE) {

    } else if(message.messagetype() == Message::MessageType::Message_MessageType_INFORMATION) {
        double allocationRatio = -1;
        for(int &&i = 0; i < message.info().records_size(); i++) {
            allocationRatio = message.info().records(i).creationratedata().shortallocationratio();
        }
        if(allocationRatio > ALLOCATION_BEFORE_GIVING_KEYSPACE) {
            for(Node* node : peers) {

                // If I know who the message is from
                if(node->getUUID() == message.sourcenodeid()) {

                    /// FIXME: Baylor, this something to consider. Right now if the keyspace is running low, then
                    /// there will be a case when we have the following scenarios below happen. These are NOT currently
                    /// covered. These was mainly fixed when we got the allocation working.
                    /// If we have 1 keyspace, and only 1 spot open in that keyspace
                    /// then do nothing.
                    /// If we have more than 1 keyspace with 1 spot open
                    /// give the entire keyspace

                    // Now we have the Node* for the peer with the message
                    if(!this->keySpace.empty()) {
                        Keyspace* keyspaceToGive = this->keySpace.at(minimumKeyspaceIndex());

                        // If i have another key spot available
                        if((keyspaceToGive->getStart() + pow(2, keyspaceToGive->getSuffix())) < keyspaceToGive->getEnd()) {
                            giveKeyspaceToNode(node, 1);
                            return true;
                        }
                    }
                    break;
                }
            }
        }
        // TODO: update peer history
    } else if(message.messagetype() == Message::MessageType::Message_MessageType_DATA_REPLICATION_UNUSED) {

    }
    return false;
}

void Node::giveKeyspaceToNode(Node* node, float percentageToGive) {
    int minKeyspaceIndex = minimumKeyspaceIndex();

    Keyspace* minKeyspace = this->keySpace.at(minKeyspaceIndex);

    unsigned long myStart = minKeyspace->getStart();
    unsigned long myEnd = minKeyspace->getEnd();
    unsigned long mySuffix = minKeyspace->getSuffix();
    mySuffix += 1;

    unsigned long newStart = minKeyspace->getStart();
    unsigned long newEnd = minKeyspace->getEnd();
    unsigned long newSuffix = minKeyspace->getSuffix();
    newStart += pow(2, newSuffix);
    newSuffix += 1;

    if(newStart > 2147483647) {
        cout << "Error" << endl;
    }

    if(newSuffix < 32) {
        auto *myKeyspace = new Keyspace(myStart, myEnd, mySuffix);
        auto *newKeyspace = new Keyspace(newStart, newEnd, newSuffix);

        if(newKeyspace->getStart() > newKeyspace->getEnd()) {
            cout << "Error" << endl;
        }
        if(myKeyspace->getStart() > myKeyspace->getEnd()) {
            cout << "Error" << endl;
        }

        this->keySpace.at(minKeyspaceIndex) = myKeyspace;
        node->keySpace.push_back(newKeyspace);
    } else {
//        cout << "ERROR we have run out of keyspace blocks, need to further sub-divide the keys using start/end" << endl;

        // Start breaking into sub-blocks!
        // (((end - start) / 2^B) / (chunkiness)) * B + start
        // B is the suffix bits

        unsigned long myStart2 = minKeyspace->getStart();
        unsigned long myEnd2 = minKeyspace->getEnd();
        unsigned long suffix = minKeyspace->getSuffix();

        double amountOfBlocks = (double) (myEnd2 - myStart2) / pow(2, suffix);
        amountOfBlocks = amountOfBlocks + 0.5 - (amountOfBlocks < 0);

        int amountOfBlocksToGive = ((int)amountOfBlocks) / CHUNKINESS;
        int blocksScaled = amountOfBlocksToGive * suffix;
        int myNewEnd = blocksScaled + myStart2;

        auto *myKeyspace = new Keyspace(myStart2, myNewEnd, suffix);
        auto *newKeyspace = new Keyspace(myNewEnd, myEnd2, suffix);

        if(newStart > 2147483647 || myNewEnd > 2147483647) {
            cout << "Error" << endl;
        }

        if(newKeyspace->getStart() > newKeyspace->getEnd()) {
            cout << "Error" << endl;
        }
        if(myKeyspace->getStart() > myKeyspace->getEnd()) {
            cout << "Error" << endl;
        }

        this->keySpace.at(minKeyspaceIndex) = myKeyspace;
        node->keySpace.push_back(newKeyspace);
    }
}

Message Node::getHeartbeatMessage() {
    float allocation = 0.0;
    if(this->keySpace.empty()) {
        allocation = 1.0;
    }
    Message msg = newBaseMessage(
            this->uuid,
            (HexDigest &) BROADCAST_UUID,
            0,
            Message::ChannelState::Message_ChannelState_NORMAL_COMMUNICATION,
            1
    );
    toInformationalMessage(
            msg,
            {
                CollectionInfoRecord{"test", 0.0, 0.0, allocation, allocation},
            }
    );
    return msg;
}

NodeData *Node::getLastDay() const {
    return lastDay;
}

void Node::setLastDay(NodeData *lastDay) {
    Node::lastDay = lastDay;
}

///**
// * computes the generation rate of a node and all its peers.
// *TODO alter funct so that it excludes the destination node in the computation
// */
//double Node::computeAggregateGenRate() {
//    double totalPeerRate;
//
//    for(int i = 0; i < peers.size(); i++) {
//        totalPeerRate += peers.at(i)->keyGenRate;
//    }
//
//    totalPeerRate = totalPeerRate * 0.30;
//
//    aggregateGenRate = totalPeerRate + keyGenRate;
//    return aggregateGenRate;
//}
///**
// * TODO write next two functions
// * I'm not certain of how to write these.
// */
//double Node::computeShortTermAllocationRatio(){
//    return -1.0;
//}
//
//double Node::computeLongTermAllocationRatio(){
//    return -1.0;
//}
//
///**
// * TODO write function
// * Requires understanding of the two above for computation
// */
//double Node::computeAggregateAllocationRatio() {
//    return -1.0;
//}
//
///**
// * TODO write function
// * found by dividing the relevant creation (gen) rate by the relevant allocation rate.
// * How do I determine which rates are the relevant ones?
// */
//double Node::computeProvisioningRatio() {
//    return -1.0;
//}
