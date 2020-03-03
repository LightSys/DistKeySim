#ifndef NETWORK
#define NETWORK

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "Node.h"
#include "UUID.h"

struct Channel{
    unsigned long long channelId;
    UUID toNode;
    UUID fromNode;
};

class Network{
public:
    Network(std::map<UUID, Node*> nodes, std::vector<Channel*> channels);

    // FIXME: change message to a Message type
    void sendMg (std::string message, UUID toNode, UUID fromNode);


private:
    std::map<UUID, Node*> nodes; //UUID, Node by reference
    std::vector<Channel*> channels;

    //This may not be needed
//    std::map<UUID, UUID> nodeConnections;
};

#endif /* network */