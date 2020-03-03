#ifndef LIGHTSYSTEMP_NODE_H
#define LIGHTSYSTEMP_NODE_H

#include <iostream>
#include <vector>

#include "UUID.h"
#include "Keyspace.h"

///TODO integrate RPC into the class
///TODO likely other things that I didn't consider.
class Node {
private:
    UUID uuid;
    Keyspace* keySpace;
    int keyShareRate;
    double keyGenRate;
    double aggregateGenRate;
    double shortTermAllocationRatio;
    double longTermAllocationRatio;
    double aggregateAllocationRatio;
    double provisioningRatio;
    bool active;
    std::vector<Node*> peers; ///called directConnection on the board.
public:
    Node();

    ///getter and setter for nodeID
    ///Alter these based on the fact that the nodeID should be be a UUID
    UUID getUUID() const { return uuid; }
    void setUUID(UUID nid) { uuid = nid; }

    ///getter and setter for keySpace
    Keyspace* getKeySpace() const { return keySpace; }
    void setKeySpace(Keyspace* ks) { keySpace = ks; }

    ///getter and setter for keyGenRate
    double getKeyGenRate() const { return keyGenRate; }
    void setKeyGenRate(double kgr) { keyGenRate = kgr; }

    /**
     *
     * @return
     */
    int getKeyShareRate() const { return keyShareRate; }
    void setKeyShareRate(int ksr) { keyShareRate = ksr; }

    /**
     *
     * @return
     */
    bool getActive() const { return active; }
    void setActive(bool a) { active = a; }

    Keyspace createKeySpace();
    double computeAggregateGenRate();
    double computeShortTermAllocationRatio();
    double computeLongTermAllocationRatio();
    double computeAggregateAllocationRatio();
    double computeProvisioningRatio();
};

#endif //LIGHTSYSTEMP_NODE_H
