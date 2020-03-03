#ifndef LIGHTSYS_ADAK_SIMULATION_H
#define LIGHTSYS_ADAK_SIMULATION_H
#include "Network.h"

class Simulation {
private:
public:
    void connectNodes(Node* nodeA, Node* nodeB);
    void disconnectNodes(Node* nodeA, Node* nodeB);

    void initiateMessage(Node* to, Node* from, std::string message);
    void allocateKeyspace(Node* to, Node* from);
};

#endif //LIGHTSYS_ADAK_SIMULATION_H