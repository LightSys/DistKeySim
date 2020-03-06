#ifndef ADAK_KEYING_NODEDATA_H
#define ADAK_KEYING_NODEDATA_H


// FIXME: make this a tunable parameter
static const double NETWORK_SCALE = 0.3;
static const double ALLOCATION_BEFORE_GIVING_KEYSPACE = 0.7;

//#include "Node.h"

class Node;

class NodeData {
private:
    int keyShareRate;
    double keyGenRate;
    double aggregateGenRate;
    double shortTermAllocationRatio;
    double longTermAllocationRatio;
    double aggregateAllocationRatio;
    double provisioningRatio;

    double creationRate;

    int keysUsed = 0;
    int day;

    Node* parentNode;
public:
    int getKeyShareRate() const;

    void setKeyShareRate(int keyShareRate);

    double getKeyGenRate() const;

    void setKeyGenRate(double keyGenRate);

    double getAggregateGenRate() const;

    void setAggregateGenRate(double aggregateGenRate);

    double getShortTermAllocationRatio() const;

    void setShortTermAllocationRatio(double shortTermAllocationRatio);

    double getLongTermAllocationRatio() const;

    void setLongTermAllocationRatio(double longTermAllocationRatio);

    double getAggregateAllocationRatio() const;

    void setAggregateAllocationRatio(double aggregateAllocationRatio);

    double getProvisioningRatio() const;

    void setProvisioningRatio(double provisioningRatio);

    void setCreationRate(double creationRate);

    void setKeysUsed(int keysUsed);

    void setDay(int day);

    Node *getParentNode() const;

    void setParentNode(Node *parentNode);

public:
    NodeData(Node* parentNode);

    static bool isNewDay(int currentDay);
    static int getCurrentDay();

    void useKey() { keysUsed++; }
    int getKeysUsed() const { return this->keysUsed; }

    double updateCreationRate();

    adak_key findEndKey(int creationRate);

    std::vector<Keyspace*> copyKeyspace(std::vector<Keyspace*> keyspaces);

    int getMinKey(std::vector<Keyspace*> keyspaces);
    // Getters and Setters
    double getCreationRate() const {return this->creationRate; }

    double updateLongTermAllocationRatio();

    double updateShortTermAllocationRatio();


//    int getKeyShareRate() const { return keyShareRate; }
//    void setKeyShareRate(int keyShareRate) { NodeData::keyShareRate = keyShareRate; }
//
//    double getKeyGenRate() const { return keyGenRate; }
//    void setKeyGenRate(double keyGenRate) { NodeData::keyGenRate = keyGenRate; }
//
//    double getAggregateGenRate();
//    void setAggregateGenRate(double aggregateGenRate) { NodeData::aggregateGenRate = aggregateGenRate; }
//
//    double getShortTermAllocationRatio() const { return shortTermAllocationRatio; }
//    void setShortTermAllocationRatio(double shortTermAllocationRatio) { NodeData::shortTermAllocationRatio = shortTermAllocationRatio; }
//
//    double getLongTermAllocationRatio() const { return longTermAllocationRatio; }
//    void setLongTermAllocationRatio(double longTermAllocationRatio) { NodeData::longTermAllocationRatio = longTermAllocationRatio; }
//
//    double getAggregateAllocationRatio() const { return aggregateAllocationRatio; }
//    void setAggregateAllocationRatio(double aggregateAllocationRatio) { NodeData::aggregateAllocationRatio = aggregateAllocationRatio; }
//
//    double getProvisioningRatio() const { return provisioningRatio; }
//    void setProvisioningRatio(double provisioningRatio) { NodeData::provisioningRatio = provisioningRatio; }



    int getDay() const { return day; }
//    void setDay(int day) { NodeData::day = day; }

};


#endif //ADAK_KEYING_NODEDATA_H
