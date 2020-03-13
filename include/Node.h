#ifndef ADAK_KEYING_NODE_H
#define ADAK_KEYING_NODE_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "UUID.h"
#include "Keyspace.h"
#include "message.hpp"
#include "NodeData.h"
#include "Logger.h"

class NodeData;

static const HexDigest BROADCAST_UUID = "00000000-0000-0000-0000-000000000000";

class Node {
private:
    UUID uuid;
    u_int messageID = 1;
    std::deque<Message> sendQueue;
    std::deque<Message> receiveQueue;
    std::vector<Keyspace> keyspaces;
    std::map<UUID, std::shared_ptr<Message>> peers;
    std::vector<NodeData> history;

    // Node statistics
    NodeData lastDay;
    float createdDay;
    float createdWeek;
    ADAK_Key_t totalLocalKeyspaceSize=0;

    /**
     * Generates the heartbeat informational message instance as per the specification
     * @param peerID UUID of peer to send to
     * @return Message with hearbeat information for this node
     */
    Message getHeartbeatMessage(const UUID &peerID) const;

public:
    Node();
    Node(const Keyspace &keyspace);
    static Node rootNode();
    ~Node() = default;

    shared_ptr<vector<NodeData>> getHistory(){
        return shared_ptr<vector<NodeData>>(&history);
    }

    shared_ptr<map<UUID, std::shared_ptr>> 

    /**
     * Adds a peer to local list of peers
     * @param peer Const reference to Node instance to add as peer
     */
    void addPeer(std::shared_ptr<Node> peer);

    /**
     * Adds a peer to local list of peers
     * @param peer Reference to UUID of peer to add
     */
    void addPeer(const UUID &peerUUID);

    /**
     * Attempts to remove peer based on pointer
     * @param peer Reference to Node instance
     */
    void removePeer(std::shared_ptr<Node> peer);

    /**
     * Attempts to remove peer based on UUID
     * @param peerUUID UUID of peer to attempt to remove
     */
    void removePeer(const UUID &peerUUID);

    // Network interaction
    /**
     * Moves all messages out of queue
     * @return Vector of all messages queued for sending
     */
    std::deque<Message> getMessages();

    // Generate heartbeat messages for all peers
    void heartbeat();

    queue<Message> getReceivedMessages();

    int minimumKeyspaceIndex();
    ADAK_Key_t getNextKey();
    bool isKeyAvailable();

    const UUID getUUID() const { return uuid; }
    void setUUID(UUID nid) { uuid = nid; }

    std::vector<Keyspace> getKeySpace() const { return keyspaces; }

//    double getKeyGenRate() const { return keyGenRate; }
//    void setKeyGenRate(double kgr) { keyGenRate = kgr; }
//
//    int getKeyShareRate() const { return keyShareRate; }
//    void setKeyShareRate(int ksr) { keyShareRate = ksr; }
//
//    bool getActive() const { return active; }
//    void setActive(bool a) { active = a; }

    const std::map<UUID, std::shared_ptr<Message>> getPeers() const { return this->peers; }

    /**
     * Retrieves latest statistics for this node
     * @return Safe shared pointer to most recent NodeData instance
     */
    std::shared_ptr<NodeData> getNodeData() const;

    /**
     * Add Keyspace Exchange message details from keyspace shared from Node's spaces
     * @param msg Base message instance to add records to
     */
    void shareKeyspace(Message &msg);

    void setTotalLocalKeyspaceSize(ADAK_Key_t newSize) {this->totalLocalKeyspaceSize = newSize;};

    ADAK_Key_t getTotalLocalKeyspaceSize() const {return this->totalLocalKeyspaceSize;};

    void logInfoForHeartbeat();
};

#endif //ADAK_KEYING_NODE_H
