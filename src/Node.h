#ifndef LIGHTSYSTEMP_NODE_H
#define LIGHTSYSTEMP_NODE_H

#include <iostream>
#include <vector>

///TODO integrate RPC into the class
///TODO likely other things that I didn't consider.
class Node {
public:
    Node();

    ///getter and setter for nodeID
    ///Alter these based on the fact that the nodeID should be be a UUID
    int getNodeID() const { return nodeID; }
    void setNodeID(int nid) { nodeID = nid; }

    ///getter and setter for keySpace
    int getKeySpace() const { return keySpace; }
    void setKeySpace(int ks) { keySpace = ks; }

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

    int createKeySpace();
    double computeAggregateGenRate();
    double computeShortTermAllocationRatio();
    double computeLongTermAllocationRatio();
    double computeAggregateAllocationRatio();
    double computeProvisioningRatio();


private:
    int nodeID; ///nodeID is a 128 bit UUID so this should presumably not be an int
    int keySpace;
    int keyShareRate;
    double keyGenRate;
    double aggregateGenRate;
    double shortTermAllocationRatio;
    double longTermAllocationRatio;
    double aggregateAllocationRatio;
    double provisioningRatio;
    bool active;
    std::vector<Node> peers; ///called directConnection on the board.
};


#endif //LIGHTSYSTEMP_NODE_H
