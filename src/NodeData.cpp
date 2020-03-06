#include <iostream>
#include <ctime>
#include <vector>


#include "Node.h"
#include "NodeData.h"

using namespace std;

NodeData::NodeData(Node* parentNode) : parentNode(parentNode) {
    day = getCurrentDay();
    keysUsed = 0;
}

NodeData::~NodeData() {
    delete parentNode;
}


bool NodeData::isNewDay(int currentDay) {
    return currentDay != getCurrentDay();
}

int NodeData::getCurrentDay() {
    // current date/time based on current system
    time_t now = time(0);

//    cout << "Number of sec since January 1,1970:" << now << endl;

    tm *ltm = localtime(&now);

//    // print various components of tm structure.
//    cout << "Year" << 1900 + ltm->tm_year << endl;
//    cout << "Month: "<< 1 + ltm->tm_mon<< endl;
//    cout << "Day: "<<  ltm->tm_mday << endl;
//    cout << "Time: "<< 1 + ltm->tm_hour << ":";
//    cout << 1 + ltm->tm_min << ":";
//    cout << 1 + ltm->tm_sec << endl;
    return ltm->tm_mday;
}

double NodeData::updateCreationRate() {
    double totalRate = 0.0;
    for(Node* node : parentNode->getPeers()) {
        totalRate += node->getNodeData()->getKeysUsed();
    }
    totalRate *= NETWORK_SCALE;
    //FIXME: This is just for testing
    creationRate = this->getKeysUsed() + totalRate;
    return this->getKeysUsed() + totalRate;
}

double NodeData::updateLongTermAllocationRatio(){
    double longTermRatio =0.0;
    for(Keyspace* keyspace: parentNode->getKeySpace()){
        longTermRatio +=  (double)(1.0 / (pow(2, keyspace->getSuffix())));
    }
    longTermAllocationRatio = longTermRatio;
    return longTermRatio;
}

double NodeData::updateShortTermAllocationRatio(){
    double shortTermRatio = 0.0;
    double tempKeys;
    updateCreationRate();
    if(creationRate < 1){
        tempKeys = 1;
    }else{
        tempKeys = creationRate;
    }

    adak_key startKey = parentNode->getKeySpace().at(parentNode->minimumKeyspaceIndex())->getStart();
    adak_key endKey = findEndKey(creationRate);

    //In theory you could get a ratio larger than one but you would need trillions of keys created by
    //one node in a day to make this happen therefore we did not deal with it
    shortTermRatio =  (double) tempKeys /  (endKey - startKey);
    return shortTermRatio;
}


adak_key NodeData::findEndKey(int creationRate){
    //Needs to be C+1
    creationRate +=1;
    std::vector<Keyspace*> copyKeyspaces = this->copyKeyspace(parentNode->getKeySpace());

    int minKeyIndex;
    adak_key endKey = -1;
    for(int i = 0; i <= creationRate; i++){
        //TODO:May need to actually fix this another way if out keyspace
        minKeyIndex = getMinKeyIndex(copyKeyspaces);
        if(minKeyIndex != -1) {
            endKey = copyKeyspaces.at(minKeyIndex)->getNextAvailableKey();
        }else{
            cout << "ERROR from findEndKey: no more keys to distribute"<<endl;
        }
    }
    for(Keyspace* key: copyKeyspaces) {
        delete key;
    }

    return endKey;
}

int NodeData::getMinKeyIndex(std::vector<Keyspace*> keyspaces){
    unsigned long min = ULONG_MAX;
    int index = -1;
    for(int i =0; i < keyspaces.size(); i++){
        if(keyspaces[i]->getStart() < min && keyspaces[i]->isKeyAvailable()){
            min = keyspaces[i]->getStart();
            index = i;
        }
    }
    return index;
}


std::vector<Keyspace*> NodeData::copyKeyspace(std::vector<Keyspace*> keyspaces){
    std::vector<Keyspace*> copyKeyspaces;

    for(Keyspace* keys: keyspaces){
        Keyspace* copyKey = new Keyspace(*keys);
        copyKeyspaces.push_back(copyKey);
    }

    return copyKeyspaces;
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
