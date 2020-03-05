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
    ~Node() = default;

    void addPeer(Node* peer) { this->peers.push_back(peer); }
    void removePeer(Node* peer); /*{ this->peers.erase(peers.begin())}*/

    void sendMessage();

    void receiveMessage(const Message message);

    /**
     * Per the specification, each node will send out a signal letting other nodes know how much
     * keyspace it is using etc. These informational messages will be generated in this class.
     * @return
     */
    Message getHeartbeatMessage();


    /**
     * Gives keyspace to a node
     * @param node
     * @param percentageToGive (Will be implemented later, allows for different percentages to be given for testing)
     */
    void giveKeyspaceToNode(Node* node, float percentageToGive);

    int minimumKeyspaceIndex();
    adak_key getNextKey();
    double computeAggregateGenRate();
    double computeShortTermAllocationRatio();
    double computeLongTermAllocationRatio();
    double computeAggregateAllocationRatio();
    double computeProvisioningRatio();

    UUID getUUID() const { return uuid; }
    void setUUID(UUID nid) { uuid = nid; }

    std::vector<Keyspace*> getKeySpace() const { return keySpace; }

    double getKeyGenRate() const { return keyGenRate; }
    void setKeyGenRate(double kgr) { keyGenRate = kgr; }

    int getKeyShareRate() const { return keyShareRate; }
    void setKeyShareRate(int ksr) { keyShareRate = ksr; }

    bool getActive() const { return active; }
    void setActive(bool a) { active = a; }

    std::vector<Node*> getPeers() const { return this->peers; }

    bool isMessageWaiting() const { return this->messageWaiting; }
    void setMessageWaiting(bool messageWaiting) { this->messageWaiting = messageWaiting; }
    Message getWaitingMessage() const { return this->messageToSend; }
};

#endif //ADAK_KEYING_NODE_H
