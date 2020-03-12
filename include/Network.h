#ifndef ADAK_KEYING_NETWORK_H
#define ADAK_KEYING_NETWORK_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <pair>

#include "Node.h"
#include "UUID.h"
#include "Channel.h"
#include "message.hpp"
#include "SystemClock.h"

/**
 * Types of connections that are possible when creating the initial network connection
 */
enum class ConnectionType { Full, Partial, Single };

class Network{
private:
    // Pairing UUID with Node instances
    std::map<UUID, std::shared_ptr<Node>> nodes;

     // All known channel between nodes, this is basically representing the edges of the graph
    std::vector<Channel> channels;
    // std::map<UUID, std::shared_ptr<Node>> incoming, outgoing;
    //node id to index into the clock timer array for the current timer
    // (time to start)
    std::map<UUID, int> goOfflineTimer;
    //node id to when the node will come back oneline timer index in the Clock class
    std::map<UUID, int> backOnlineTimer;
    //next timer id to use
    int nextTimerID = 0;

    ClockType clock = new Clock();

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
     * @return Boolean indicating whether the message sent passed keyspace (HACK: needed since this is non-threaded)
     */
    bool sendMsg(const Message &message);

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
     * Adds the initial node to the Network with max keyspace
     * @return UUID of created node
     */
    UUID addRootNode();

    /**
     * Adds node to network without any keyspace
     * @return UUID of created node
     */
    UUID addEmptyNode();

    /**
     * The typical way to add a node to the network
     * @param keyspace Keyspace to give to node
     * @return UUID of created node
     */
    UUID addNode(const Keyspace &keyspace);

    /**
     * Make connections within network for new node
     * @param newNode Node to connect
     */
    void connectNodeToNetwork(std::shared_ptr<Node> newNode);

    /**
     * Fully connects all the nodes to each other according to the ConnectionType
     * @param node Node to connect
     */
    void fullyConnect(std::shared_ptr<Node> node);

    /**
     * Connects the node to a single other node
     * @param node Node to connect
     */
    void singleConnect(std::shared_ptr<Node> node);

    /**
     * Checks to see if the channel already exists between two nodes
     * @param nodeOne Node to check for connection with
     * @param nodeTwo Node to check for connection with
     * @return bool indicating existence of channel between the two nodes
     */
    bool channelExists(const UUID &nodeOne, const UUID &nodeTwo);

    /**
     * Retrieves the ID within the network for the channel between the two nodes
     * @param nodeOne Node to retrieve connection with
     * @param nodeTwo Node to retrieve connection with
     * @return int of index of channel between the two nodes, returning -1 if there is none
     */
    int getChannelIndex(const UUID &nodeOne, const UUID &nodeTwo);

    // Checks all nodes for messages and passes on those messages to all other clients
    void checkAndSendAllNodes();

    // Sends heartbeat for all nodes
    void doAllHeartbeat();

    //sends a single node offline
    void sendOffline(UUID nodeUUID, double timeToDisconnect, double timeOffline);

    //checks if a node is offline
    bool isOffline(UUID nodeID);

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

    // Number of nodes
    inline size_t numNodes() const { return nodes.size(); }
};

#endif // ADAK_KEYING_NETWORK_H
