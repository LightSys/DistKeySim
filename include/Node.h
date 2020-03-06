#ifndef ADAK_KEYING_NODE_H
#define ADAK_KEYING_NODE_H

#include <iostream>
#include <vector>
#include <queue>

#include "UUID.h"
#include "Keyspace.h"
#include "message.hpp"
#include "NodeData.h"

class NodeData;

static const HexDigest BROADCAST_UUID = "00000000-0000-0000-0000-000000000000";

class Node {
private:
    UUID uuid;
    u_int messageID = 1;
    std::queue<Message> sendQueue;
    std::vector<Keyspace> keyspaces;
    std::map<UUID, u_int> peers;
    std::vector<NodeData> history;
    
    // Node statistics
    NodeData lastDay;
    float createdDay;
    float createdWeek;

public:
    Node();
    Node(const Keyspace &keyspace);
    ~Node() = default;

    /**
     * Adds a peer to local list of peers
     * @param peer Const reference to Node instance to add as peer
     */
    void addPeer(const Node &peer);
    
    /**
     * Adds a peer to local list of peers
     * @param peer Reference to UUID of peer to add
     */
    void addPeer(const UUID &peerUUID);
    
    /**
     * Attempts to remove peer based on pointer
     * @param peer Reference to Node instance
     */
    void removePeer(const Node &peer);
    
    /**
     * Attempts to remove peer based on UUID
     * @param peerUUID UUID of peer to attempt to remove
     */
    void removePeer(const UUID &peerUUID);
    
    // Network interaction
    
    // Generate heartbeat messages for all peers
    void heartbeat();
    bool receiveMessage(const Message &message);

    /**
     * Generates the heartbeat informational message instance as per the specification
     * @param peerID UUID of peer to send to
     * @return Message with hearbeat information for this node
     */
    Message getHeartbeatMessage(const UUID &peerID) const;
    
    int minimumKeyspaceIndex();
    adak_key getNextKey();
    
    const UUID getUUID() const { return uuid; }
    void setUUID(UUID nid) { uuid = nid; }

    std::vector<Keyspace> getKeySpace() const { return keyspaces; }

//    double getKeyGenRate() const { return keyGenRate; }
//    void setKeyGenRate(double kgr) { keyGenRate = kgr; }
//
//    int getKeyShareRate() const { return keyShareRate; }
//    void setKeyShareRate(int ksr) { keyShareRate = ksr; }

    const std::map<UUID, u_int> getPeers() const { return this->peers; }

    /**
     * Retrieves latest statistics for this node
     * @return Constant pointer to most recent NodeData instance
     */
    const NodeData* getNodeData() const;
    
    /**
     * Add Keyspace Exchange message details from keyspace shared from Node's spaces
     * @param msg Base message instance to add records to
     */
    void shareKeyspace(Message &msg);
};

#endif //ADAK_KEYING_NODE_H
