#include <iostream>
#include <ctime>
#include <vector>

#include "areCloseEnough.h"
#include "Node.h"
#include "NodeData.h"

using namespace std;

int NodeData::getCurrentDay() {
    // current date/time based on current system
    time_t now = time(0);
    tm *ltm = localtime(&now);

    return ltm->tm_mday;
}



double NodeData::updateLongTermAllocationRatio(vector<Keyspace> &keyspace) {
    double longTermRatio = 0.0;
    for (const auto &keyspace : keyspace) {
        Logger::log(Formatter() << "updateLongTermAllocationRatio: keyspace.getPercent=" << keyspace.getPercent());
        longTermRatio +=  keyspace.getPercent();
        Logger::log(Formatter() << "updateLongTermAllocationRatio: new longTermRatio=" << longTermRatio);
    }

    longTermAllocationRatio = longTermRatio;
    Logger::log(Formatter() << "updateLongTermAllocationRatio: return longTermRatio=" << longTermRatio);
    return longTermRatio;
}

double NodeData::updateShortTermAllocationRatio(const vector<Keyspace> &keyspace){
    double tempKeys = keysUsed > 0 ? keysUsed : 1;

    int minIndex = getMinKeyIndex(keyspace);
    Logger::log(Formatter() << "updateShortTermAllocationRatio: minIndex=" << minIndex);
    if(minIndex == -1) return -1;
    ADAK_Key_t startKey = keyspace.at(minIndex).getStart();
    ADAK_Key_t endKey = keyspace.size() == 0 ? -2 : UINT_MAX; //if you have any keys, must have a full block
                                                              //if you have a full block, UINT_MAX is your last key

    ///-2 means you ran out of keyspace thus set the ratio to one saying we need more keyspace right away.
    ///Baylor you may want to change this if you want a different value saying I need help right away.
    Logger::log(Formatter() << "updateShortTermAllocationRatio: startKey=" << startKey << " endKey=" << endKey);
    if(endKey == -2) {
        shortTermAllocationRatio.addValue(1);
        Logger::log(Formatter() << "updateShortTermAllocationRatio:"
            << " shortTermAllocationRatio=" << shortTermAllocationRatio.getValue());
    }
    ///This is the normal case that works for long term allocation.
    else {
        //In theory you could get a ratio larger than one but you would need trillions of keys created by
        //one node in a day to make this happen therefore we did not deal with it
        shortTermAllocationRatio.addValue((double) tempKeys /  (endKey - startKey));
        if (ACE::areCloseEnough(shortTermAllocationRatio.getValue(), 0)) {
            shortTermAllocationRatio.addValue(0);
        }
        Logger::log(Formatter() << "updateShortTermAllocationRatio:"
            << " tempKeys(" << tempKeys << ") / (endKey(" << endKey << ") - startKey(" << startKey << ")) = "
            << shortTermAllocationRatio.getValue());
    }
    return shortTermAllocationRatio.getValue();
}

double NodeData::updateProvisioningRatio(double creationRate, double shortTermRatio) {
    provisioningRatio = creationRate*shortTermRatio;
    return provisioningRatio;
}


ADAK_Key_t NodeData::findEndKey(double creationRate, vector<Keyspace> keyspaces){
    //Needs to be C+1
    creationRate += 1;
    std::vector<Keyspace> copyKeyspaces = keyspaces;

    int minKeyIndex;
    ADAK_Key_t endKey = -1;
    for (int i = 0; i <= creationRate; i++){
        //TODO:May need to actually fix this another way if out keyspace
        minKeyIndex = getMinKeyIndex(copyKeyspaces);
        if(minKeyIndex != -1) {
            endKey = copyKeyspaces.at(minKeyIndex).getNextAvailableKey();
        }else{
            ///Baylor If this error occurs that is probably not a real issue in the code. It probably has to do that there
            ///is more keys being made in a day than allocated in the keyspace. You should fix your algorithm to
            ///distrute keys faster
            string message = "ERROR from findEndKey: no more keys to distribute";
            Logger::log(message);
            ///Error that you are really out of keys
            endKey = -2;
        }
    }

    return endKey;
}

int NodeData::getMinKeyIndex(const std::vector<Keyspace> keyspaces) const {
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
    return shortTermAllocationRatio.getValue();
}

void NodeData::setShortTermAllocationRatio(double shortTermAllocationRatio) {
    NodeData::shortTermAllocationRatio.addValue(shortTermAllocationRatio);
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
