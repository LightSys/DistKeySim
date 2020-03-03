#ifndef NETWORK
#define NETWORK

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "Node.h"

struct Channel{
    unsigned long long channelId;
    std::string toNodeUUID;
    std::string fromNodeUUID;
};

class Network{
public:
    Network(std::map<std::string, Node*> nodes, std::vector<Channel*> channels);

    // FIXME: change message to a Message type
    void sendMg (std::string message, std::string toNodeUUID, std::string fromNodeUUID);


private:
    std::map<std::string, Node*> nodes; //UUID, Node by reference
    std::vector<Channel*> channels;

    //This may not be needed
//    std::map<UUID, UUID> nodeConnections;
};

#endif /* network */