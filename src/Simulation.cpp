#include <iostream>

#include "Simulation.h"
#include "UUID.h"
#include "EventGen.h"
#include "Random.h"

using namespace std;

Simulation::Simulation() {
    // Seed random number
    srand(time(NULL));
    this->network = new Network();
}

int main(int argc, char **argv){

    EventGen* eventGenerator = new Random();
    Simulation* simulation = new Simulation();
    Network* network = simulation->getNetwork();

    // Create new nodes and add them to the map
    for(int i = 0; i < AMOUNT_OF_NODES; i++) {
        network->addNode(new Node());
    }

    for(int i = 0; i < 10; i++) {
        eventGenerator->eventTick(network);
    }
    
    std::ofstream csvFile;
    csvFile.open("csvFile.csv", std::ofstream::out | std::ofstream::trunc);
    csvFile << "UUID, keySpace, keyGenRate, active, provisioningRatio \n";
    for (int i = 0; i < (network->getUUIDList().size()); i++){
        UUID uuid = network->getUUIDList().at(i); ///will get the UUID
        Keyspace kS = network->getNodeFromUUID(uuid)->getKeySpace();
        double kGR = network->getNodeFromUUID(uuid)->getKeyGenRate();
        bool active = network->getNodeFromUUID(uuid)->getActive();
        std::string act = "";
        if(active == true) {
            act = "true";
        }
        else{
            act = "false";
        }
        double pR = network->getNodeFromUUID(uuid)->getProvisioningRatio();
        uuid = UUIDToHex(network->getUUIDList().at(i));
        csvFile << uuid << ", " << kS << ", " << kGR << ", " << act << ", " << pR << "\n";
    }
    csvFile.close();

    network->printUUIDList();
    network->printChannels();
}
