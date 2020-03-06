#include <iostream>
#include <ctime>
#include <vector>

#include "Node.h"
#include "NodeData.h"

using namespace std;

int NodeData::getCurrentDay() {
    // current date/time based on current system
    time_t now = time(0);
    tm *ltm = localtime(&now);

    return ltm->tm_mday;
}

double NodeData::updateCreationRate(const map<UUID, shared_ptr<Message>> peers) {
    double totalRate = 0.0;
    for (const auto &[_, peer] : peers) {
        totalRate += peer->info().records(0).creationratedata().shortallocationratio();
    }
    
    totalRate *= NETWORK_SCALE;
    creationRate = this->getKeysUsed() + totalRate;
    
    return creationRate;
}

double NodeData::updateLongTermAllocationRatio(const vector<Keyspace> &keyspace) {
    double longTermRatio = 0.0;
    for (const auto &keyspace : keyspace) {
        longTermRatio +=  static_cast<double>(1.0 / pow(2, keyspace.getSuffix()));
    }
    
    longTermAllocationRatio = longTermRatio;
    return longTermRatio;
}

double NodeData::updateShortTermAllocationRatio(const vector<Keyspace> &keyspace, u_int minKeyspaceIdx,
                                                const map<UUID, shared_ptr<Message>> &peers) {
    int tempKeys = creationRate > 0 ? creationRate : 1;

    ADAK_Key_t startKey = keyspace.at(minKeyspaceIdx).getStart();
    ADAK_Key_t endKey = findEndKey(updateCreationRate(peers), keyspace);
    
    // In theory you could get a ratio larger than one but you would need millions of keys created by
    // one node in a day to make this happen therefore we did not deal with it
    return static_cast<double>(tempKeys) /  (endKey - startKey);
}


ADAK_Key_t NodeData::findEndKey(double creationRate, vector<Keyspace> keyspaces){
    //Needs to be C+1
    creationRate += 1;
    std::vector<Keyspace> copyKeyspaces = keyspaces;

    int minKeyIndex;
    ADAK_Key_t endKey = -1;
    for (int i = 0; i < creationRate; i++){
        //TODO:May need to actually fix this another way if out keyspace
        minKeyIndex = getMinKey(copyKeyspaces);
        if (minKeyIndex != -1) {
            endKey = copyKeyspaces.at(minKeyIndex).getNextAvailableKey();
        } else {
            cout << "ERROR from findEndKey: no more keys to distribute";
        }
    }

    return endKey;
}

int NodeData::getMinKey(const std::vector<Keyspace> keyspaces) const {
    unsigned long min = ULONG_MAX;
    int index = -1;
    for (int i = 0; i < keyspaces.size(); i++){
        if (keyspaces.at(i).getStart() < min && keyspaces.at(i).isKeyAvailable()){
            min = keyspaces.at(i).getStart();
            index = i;
        }
    }
    return index;
}

int NodeData::getKeyShareRate() const {
    return keyShareRate;
}

void NodeData::setKeyShareRate(int keyShareRate) {
    NodeData::keyShareRate = keyShareRate;
}

double NodeData::getKeyGenRate() const {
    return keyGenRate;
}

void NodeData::setKeyGenRate(double keyGenRate) {
    NodeData::keyGenRate = keyGenRate;
}

double NodeData::getAggregateGenRate() const {
    return aggregateGenRate;
}

void NodeData::setAggregateGenRate(double aggregateGenRate) {
    NodeData::aggregateGenRate = aggregateGenRate;
}

double NodeData::getShortTermAllocationRatio() const {
    return shortTermAllocationRatio;
}

void NodeData::setShortTermAllocationRatio(double shortTermAllocationRatio) {
    NodeData::shortTermAllocationRatio = shortTermAllocationRatio;
}

double NodeData::getLongTermAllocationRatio() const {
    return longTermAllocationRatio;
}

void NodeData::setLongTermAllocationRatio(double longTermAllocationRatio) {
    NodeData::longTermAllocationRatio = longTermAllocationRatio;
}

double NodeData::getAggregateAllocationRatio() const {
    return aggregateAllocationRatio;
}

void NodeData::setAggregateAllocationRatio(double aggregateAllocationRatio) {
    NodeData::aggregateAllocationRatio = aggregateAllocationRatio;
}

double NodeData::getProvisioningRatio() const {
    return provisioningRatio;
}

void NodeData::setProvisioningRatio(double provisioningRatio) {
    NodeData::provisioningRatio = provisioningRatio;
}

void NodeData::setCreationRate(double creationRate) {
    NodeData::creationRate = creationRate;
}

void NodeData::setKeysUsed(int keysUsed) {
    NodeData::keysUsed = keysUsed;
}

void NodeData::setDay(int day) {
    NodeData::day = day;
}

Node *NodeData::getParentNode() const {
    return parentNode;
}

void NodeData::setParentNode(Node *parentNode) {
    NodeData::parentNode = parentNode;
}
