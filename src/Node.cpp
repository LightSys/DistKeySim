#include "Node.h"
#include "UUID.h"

Node::Node(){
    this->uuid = new_uuid();
}
///creates the key space for the Node
Keyspace Node::createKeySpace() {

}

/**
 * computes the generation rate of a node and all its peers.
 *TODO alter funct so that it excludes the destination node in the computation
 */
double Node::computeAggregateGenRate() {
    double totalPeerRate;

    for (int i = 0; i < peers.size(); i++) {
        totalPeerRate += peers.at(i).keyGenRate;
    }

    totalPeerRate = totalPeerRate * 0.30;

    aggregateGenRate = totalPeerRate + keyGenRate;
}
/**
 * TODO write next two functions
 * I'm not certain of how to write these.
 */
double computeShortTermAllocationRatio(){

}

double computeLongTermAllocationRatio(){

}

/**
 * TODO write function
 * Requires understanding of the two above for computation
 */
double computeAggregateAllocationRatio() {

}

/**
 * TODO write function
 * found by dividing the relevant creation (gen) rate by the relevant allocation rate.
 * How do I determine which rates are the relevant ones?
 */
double computeProvisioningRatio() {

}
