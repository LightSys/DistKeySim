#include <ctime>

#include "GeometricDisconnect.h"

using namespace std;

GeometricDisconnect::Random() : EventGen() {
    // Seed Random
    cout << "Random Class Created... Seeding random number." << endl;
    srand(time(NULL));
}

void GeometricDisconnect::eventTick(Network* network, double lambda1, double lambda2) {
    int action = rand() % 3 + 1; // random number between 1-3
    UUID randomUUID = network->getRandomNode();
    shared_ptr<Node> node = network->getNodeFromUUID(randomUUID);
    switch(action) {
        case 1:
            // Uses key from a random node
            if(node->isKeyAvailable()) {
                node->getNextKey();
            }
            break;
        case 2:
            // Here, using CPP libraries to randomly generate value from GEOM(lambda1, TIMESTEP)
            // and from GEOM(lamda2, TIMESTEP), and use that toe connect/disconnect the nodes.
            // Note: TIMESTEP is a constant defined in GemoetricDisconnect.h


            /* PREVIOUS CODE FROM RANDOM.CPP
            network->connectNodes(network->getRandomNode(), network->getRandomNode());
            break;
        case 3:
//            network->disconnectNodes(network->getRandomNode(), network->getRandomNode());
            break;*/
        default:
            cout << "Something wrong with random!" << endl;
            break;
    }
}
