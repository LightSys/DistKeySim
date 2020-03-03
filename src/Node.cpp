#include "Node.h"
#include "UUID.h"

Node::Node() {
    this->uuid = new_uuid();

    // FIXME: set to actual keyspace
    this->keySpace.push_back(new Keyspace(0, ULONG_MAX, 0));
}

Node::Node(Keyspace* keySpace) {
    this->uuid = new_uuid();
    this->keySpace.push_back(keySpace);
}


///creates the key space for the Node
unsigned long Node::getNextKey(){
    return minimumKeyspace()->getNextAvailableKey();
}

Keyspace* Node::minimumKeyspace() {
    long min = ULONG_MAX;
    Keyspace* keySpaceMin = NULL;

    for(int i =0; i < keySpace.size(); i++){
        if(keySpace[i]->getStart() < min){
            min = keySpace[i]->getStart();
            keySpaceMin = keySpace[i];
        }
    }

    return keySpaceMin;
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
