#include <climits>
#include <iostream>

#include "Node.h"
#include "UUID.h"
#include "message.hpp"

using namespace std;

static const HexDigest& BROADCAST_UUID = "00000000-0000-0000-0000000000";

Node::Node() {
    // TODO: figure out how to call Node(Keyspace* keySpace) constructor
    Node(new Keyspace(0, ULONG_MAX, 0));
}

Node::Node(Keyspace* keySpace) {
    this->uuid = new_uuid();

    if(keySpace == nullptr) {
        this->messageWaiting = true;
        this->messageToSend = newBaseMessage(
                this->uuid,
                (HexDigest &) BROADCAST_UUID,
                0,
                Message::ChannelState::Message_ChannelState_INITIAL_STARTUP,
                1
        );
        toKeyspaceMessage(
                this->messageToSend,
                {
                        KeyspaceExchangeRecord{"test", 0, 0, 0},
                }
        );
    } else {
        this->keySpace.push_back(keySpace);
    }
}

///creates the key space for the Node
unsigned long Node::getNextKey() {
//    return minimumKeyspace()->getNextAvailableKey();
}

int Node::minimumKeyspace() {
    unsigned long min = ULONG_MAX;
    int index = -1;
//    Keyspace* keySpaceMin = NULL;

    for(int i =0; i < keySpace.size(); i++){
        if(keySpace[i]->getStart() < min){
            min = keySpace[i]->getStart();
//            keySpaceMin = keySpace[i];
            index = i;
        }
    }
    return index;
}

/**
 * This is where the Baylor team will add more statistics to handle the messages that each node receives
 * @param message
 */
void Node::receiveMessage(const Message message) {

    if(message.messagetype() == Message::MessageType::Message_MessageType_KEYSPACE) {

        // If the node is addressed to me
        if(message.destnodeid() == this->uuid || message.destnodeid() == BROADCAST_UUID) {
            for(Node* node : peers) {

                // If I know who the message is from
                if(node->getUUID() == message.sourcenodeid()) {

                    // Need to decide when I give keyspace, for right now, we will automatically give the keyspace
                    if(!this->keySpace.empty()) {
                        giveKeyspaceToNode(node, 1);

                        // Also need to send the ACK
                        break;
                    }
                }
            }
        }
    } else if(message.messagetype() == Message::MessageType::Message_MessageType_INFORMATION) {

    } else if(message.messagetype() == Message::MessageType::Message_MessageType_DATA_REPLICATION_UNUSED) {

    }
}

void Node::giveKeyspaceToNode(Node* node, float percentageToGive) {
    int minKeyspaceIndex = minimumKeyspace();

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

    if(newSuffix <= 32) {
        auto *myKeyspace = new Keyspace(myStart, myEnd, mySuffix);
        auto *newKeyspace = new Keyspace(newStart, newEnd, newSuffix);

        this->keySpace.at(minKeyspaceIndex) = myKeyspace;
        node->keySpace.push_back(newKeyspace);
    } else {
        cout << "ERROR we have run out of keyspace blocks, need to further sub-divide the keys using start/end" << endl;
    }
}

/**
 * computes the generation rate of a node and all its peers.
 *TODO alter funct so that it excludes the destination node in the computation
 */
double Node::computeAggregateGenRate() {
    double totalPeerRate;

    for(int i = 0; i < peers.size(); i++) {
        totalPeerRate += peers.at(i)->keyGenRate;
    }

    totalPeerRate = totalPeerRate * 0.30;

    aggregateGenRate = totalPeerRate + keyGenRate;
    return aggregateGenRate;
}
/**
 * TODO write next two functions
 * I'm not certain of how to write these.
 */
double Node::computeShortTermAllocationRatio(){
    return -1.0;
}

double Node::computeLongTermAllocationRatio(){
    return -1.0;
}

/**
 * TODO write function
 * Requires understanding of the two above for computation
 */
double Node::computeAggregateAllocationRatio() {
    return -1.0;
}

/**
 * TODO write function
 * found by dividing the relevant creation (gen) rate by the relevant allocation rate.
 * How do I determine which rates are the relevant ones?
 */
double Node::computeProvisioningRatio() {
    return -1.0;
}
