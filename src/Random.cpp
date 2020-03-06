#include <ctime>

#include "Random.h"

using namespace std;

Random::Random() : EventGen() {
    // Seed Random
    cout << "Random Class Created... Seeding random number." << endl;
    srand(time(NULL));
}

void Random::eventTick(Network* network) {
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
            network->connectNodes(network->getRandomNode(), network->getRandomNode());
            break;
        case 3:
//            network->disconnectNodes(network->getRandomNode(), network->getRandomNode());
            break;
        default:
            cout << "Something wrong with random!" << endl;
            break;
    }
}