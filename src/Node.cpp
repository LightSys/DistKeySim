#include "Node.h"
#include "UUID.h"

Node::Node() {
    this->uuid = new_uuid();

    // FIXME: set to actual keyspace
    this->keySpace.push_back(new Keyspace(0, INT32_MAX, 0));
}

Node::Node(Keyspace* keySpace) {
    this->uuid = new_uuid();
    this->keySpace.push_back(keySpace);
}

Node::~Node() {
    for(Keyspace* keyspace : keySpace) {
        delete keyspace;
    }
}

void Node::addPeer(Node *peer) {
    this->peers.push_back(peer);
}

/**
 * computes the generation rate of a node and all its peers.
 *TODO alter funct so that it excludes the destination node in the computation
 */
double Node::computeAggregateGenRate() {
    double totalPeerRate;

    for (int i = 0; i < peers.size(); i++) {
        totalPeerRate += peers.at(i)->keyGenRate;
    }

    totalPeerRate = totalPeerRate * 0.30;

    aggregateGenRate = totalPeerRate + keyGenRate;
}
/**
 * TODO write next two functions
 * I'm not certain of how to write these.
 */
double Node::computeShortTermAllocationRatio(){

}

double Node::computeLongTermAllocationRatio(){

}

/**
 * TODO write function
 * Requires understanding of the two above for computation
 */
double Node::computeAggregateAllocationRatio() {

}

/**
 * TODO write function
 * found by dividing the relevant creation (gen) rate by the relevant allocation rate.
 * How do I determine which rates are the relevant ones?
 */
double Node::computeProvisioningRatio() {

}
