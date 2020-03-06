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
enum class ConnectionType { Full, Partial, Single};

class Network{
private:
    // Pairing UUID with Node instances
    std::map<UUID, std::shared_ptr<Node>> nodes;

     // All known channel between nodes, this is basically representing the edges of the graph
    std::vector<Channel> channels;

    // Connection type used to connect member Nodes
    ConnectionType connectionType;
public:
    Network(ConnectionType connectionType);
    ~Network() = default;

    /**
     * Connects two nodes by creating a channel between them and connecting the nodes based on the type of connection
     * @param nodeOne UUID of first node to connect to
     * @param nodeTwo UUID of second node to connect to
     */
    void connectNodes(const UUID &nodeOne, const UUID &nodeTwo);

    /**
     * Disconnects the two nodes and removes the existing channel between them
     * @param nodeOne UUID of first node to disconnect
     * @param nodeTwo UUID of second node to disconnect
     */
    void disconnectNodes(const UUID &nodeOne, const UUID &nodeTwo);

    /**
     * Sends message to Node, based on destination UUID
     * @param message Message to send
     */
    void sendMsg(const Message &message);

    /**
     * Returns a random node from the Node list, useful for creating random events
     * @return UUID of random node within network
     */
    UUID getRandomNode();

    /**
     * Converts the UUID to the associated Node*
     * @param uuid UUID of Node to retrieve
     * @return Safe pointer to Node
     */
    std::shared_ptr<Node> getNodeFromUUID(const UUID &uuid) const;

    /**
     * Adds the initial node to the Network, this defaults to the max Keyspace, so it should only be used once.
     */
    UUID addNode();

    /**
     * The typical way to add a node to the network
     * @param keyspace
     */
    UUID addNode(const Keyspace &keyspace);

    /**
     * Fully connects all the nodes to each other according to the ConnectionType
     * @param node
     */
    void fullyConnect(std::shared_ptr<Node> &node);

    void singleConnect(std::shared_ptr<Node> &node);

    /**
     * Checks to see if the channel already exists between two nodes,
     * if the channel exists return true, false otherwise.
     * @param nodeOne
     * @param nodeTwo
     * @return bool (channelExists)
     */
    bool channelExists(const UUID &nodeOne, const UUID &nodeTwo);
    int getChannelIndex(const UUID &nodeOne, const UUID &nodeTwo);

    // Checks all nodes for messages and passes on those messages to all other clients
    void checkAndSendAllNodes();

    /**
     * Generates a UUID list based on the known UUIDs from the map<UUID, Node*>
     * @return List of all UUIDs for the network
     */
    std::vector<UUID> generateUUIDList();

    // Getters
    std::map<UUID, std::shared_ptr<Node>> getNodes() { return this->nodes; }
    std::vector<Channel> getChannels() { return this->channels; }

    // Output to cout
    void printUUIDList();
    void printChannels();
    void printKeyspaces();
    
    // Output to ostream, allowing for file output
    void printUUIDList(std::ostream &out, char spacer = ',');
    void printChannels(std::ostream &out, char spacer = ',');
    void printKeyspaces(std::ostream &out, char spacer = ',');
};

#endif // ADAK_KEYING_NETWORK_H
