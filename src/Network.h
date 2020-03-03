#ifndef NETWORK
#define NETWORK

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "Node.h"
#include "UUID.h"
#include "Channel.h"


class Network{
private:
    std::vector<UUID> uuidList;
    std::map<UUID, Node*> nodes; //UUID, Node by reference
    std::vector<Channel*> channels;

    //This may not be needed
//    std::map<UUID, UUID> nodeConnections;
public:
//    Network(std::map<UUID, Node*> nodes, std::vector<Channel*> channels);
    Network();
    ~Network();

    void connectNodes(UUID nodeA, UUID nodeB);
    void disconnectNodes(UUID nodeA, UUID nodeB);

    void initiateMessage(UUID to, UUID from, std::string message);
    void allocateKeyspace(UUID to, UUID from);

    // FIXME: change message to a Message type
    void sendMsg(std::string message, UUID toNode, UUID fromNode);
    void broadcastMsg(std::string message, UUID fromNode);

    UUID getRandomNode();
    Node* getNodeFromUUID(UUID uuid) const { return nodes.find(uuid)->second; }

    void addNode();
    void addNode(Keyspace* keyspace);

    // Getters
    std::map<UUID, Node*> getNodes() { return this->nodes; }
    std::vector<Channel*> getChannels() { return this->channels; }
    std::vector<UUID> getUUIDList() { return this->uuidList; }

    // Printing
    void printUUIDList();
    void printNodeList();
    void printChannels();
};

#endif /* network */