#ifndef ADAK_KEYING_NETWORK_H
#define ADAK_KEYING_NETWORK_H

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "Node.h"
#include "UUID.h"
#include "Channel.h"
#include "message.hpp"

/**
 * Types of connections that are possible when creating the initial network connection
 */
enum class ConnectionType { Full, Partial, Circular, Single};

class Network{
public:
    Network(ConnectionType connectionType);
    ~Network();

    /**
     * Connects two nodes by creating a channel between them and connecting the nodes based on the type of connection
     * @param nodeOne
     * @param nodeTwo
     */
    void connectNodes(UUID nodeOne, UUID nodeTwo);

    /**
     * Disconnects the two nodes and removes the existing channel between them
     * @param nodeOne
     * @param nodeTwo
     */
    void disconnectNodes(UUID nodeOne, UUID nodeTwo);

    void initiateMessage(UUID to, UUID from, std::string message);
    void allocateKeyspace(UUID to, UUID from);

    // FIXME: change message to a Message type
    void sendMsg(Message msg);
//    void broadcastMsg(std::string message, UUID fromNode);

    /**
     * Returns a random node from the Node list, useful for creating random events
     * @return
     */
    UUID getRandomNode();

    /**
     * Converts the UUID to the associated Node*
     * @param uuid
     * @return Node* (associated Node)
     */
    Node* getNodeFromUUID(UUID uuid) const { return nodes.find(uuid)->second; }

    /**
     * Adds the initial node to the Network, this defaults to the max Keyspace, so it should only be used once.
     */
    void addNode();

    /**
     * The typical way to add a node to the network
     * @param keyspace
     */
    void addNode(Keyspace* keyspace);

    /**
     * Fully connects all the nodes to each other according to the ConnectionType
     * @param node
     */
    void fullyConnect(Node* node);

    /**
     * Checks to see if the channel already exists between two nodes,
     * if the channel exists return true, false otherwise.
     * @param nodeOne
     * @param nodeTwo
     * @return bool (channelExists)
     */
    bool channelExists(UUID nodeOne, UUID nodeTwo);

    /**
     * Generates a UUID list based on the known UUIDs from the map<UUID, Node*>
     * @return
     */
    std::vector<UUID> generateUUIDList();

    // Getters
    std::map<UUID, Node*> getNodes() { return this->nodes; }
    std::vector<Channel*> getChannels() { return this->channels; }

    // Printing
    void printUUIDList();
    void printChannels();
private:
    /**
     * Map of connecting a UUID to a given Node*
     */
    std::map<UUID, Node*> nodes;
    /**
     * List of every known channel between nodes, this is basically representing the edges of the graph
     */
    std::vector<Channel*> channels;
    
    /**
     * The type of connection for this network
     */
    ConnectionType connectionType;
};

#endif // ADAK_KEYING_NETWORK_H