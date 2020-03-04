#ifndef ADAK_KEYING_NODE_H
#define ADAK_KEYING_NODE_H

#include <iostream>
#include <vector>

#include "UUID.h"
#include "Keyspace.h"
#include "message.hpp"

///TODO integrate RPC into the class
///TODO likely other things that I didn't consider.
class Node {
private:
    UUID uuid;
    std::vector<Keyspace*> keySpace;
    int keyShareRate;
    double keyGenRate;
    double aggregateGenRate;
    double shortTermAllocationRatio;
    double longTermAllocationRatio;
    double aggregateAllocationRatio;
    double provisioningRatio;
    bool active = true;
    bool messageWaiting = false;
    std::vector<Node*> peers; ///called directConnection on the board.

    Message messageToSend;
public:
    Node();
    Node(Keyspace* keySpace);
    ~Node() {}

    void addPeer(Node* peer) { this->peers.push_back(peer); }

    ///getter and setter for nodeID
    ///Alter these based on the fact that the nodeID should be be a UUID
    UUID getUUID() const { return uuid; }
    void setUUID(UUID nid) { uuid = nid; }

    ///getter and setter for keySpace
    std::vector<Keyspace*> getKeySpace() const { return keySpace; }

    ///Be carefull this just add a keyspace does not make a whole new one.
    void addKeySpace(Keyspace* ks) { keySpace.push_back(ks); }

    ///getter and setter for keyGenRate
    double getKeyGenRate() const { return keyGenRate; }
    void setKeyGenRate(double kgr) { keyGenRate = kgr; }

    std::vector<Node*> getPeers() const { return this->peers; }

    /**
     *
     * @return
     */
    int getKeyShareRate() const { return keyShareRate; }
    void setKeyShareRate(int ksr) { keyShareRate = ksr; }

    void sendMessage();

    // FIXME: change to Message
    void receiveMessage(Message message);

    /**
     * Gives keyspace to a node
     * @param node
     * @param percentageToGive (Will be implemented later, allows for different percentages to be given for testing)
     */
    void giveKeyspaceToNode(Node* node, float percentageToGive);

    /**
     *
     * @return
     */
    bool getActive() const { return active; }
    void setActive(bool a) { active = a; }

    int minimumKeyspace();
    unsigned long getNextKey();
    double computeAggregateGenRate();
    double computeShortTermAllocationRatio();
    double computeLongTermAllocationRatio();
    double computeAggregateAllocationRatio();
    double computeProvisioningRatio();


    bool isMessageWaiting() const { return this->messageWaiting; }
    Message getWaitingMessage() const { return this->messageToSend; }
};

#endif //ADAK_KEYING_NODE_H
