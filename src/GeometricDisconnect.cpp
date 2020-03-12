#include <ctime>
#include <random>

#include "GeometricDisconnect.h"

using namespace std;

GeometricDisconnect(ClockType clockType, double lambda1, double lambda2){
    clock = SystemClock.makeClock(clockType);
    this->lambda1 = lambda1;
    this->lambda2 = lambda2;
}

void GeometricDisconnect::eventTick(Network* network) {
    // random number between 1-10
    int totalNodes = rand() % 10 + 1;
    // d1 is the model used to randomly generate the amount of time from the
    // current time step when the node will go offline
    geometric_distribution<> d1(lambda1);
    // d2 is the model used to randomly generate the amount of time the current
    // node will remain offline
    geometric_distribution<> d2(lambda2);
    // These are required for the geometric distribution function to operate
    // correctly
    random_device rd;
    // Save the seed for debugging
    unsigned int seed = rd();
    mt19937 gen(seed);


    for (int i = 0; i < totalNodes; i ++){
      UUID randomUUID = network->getRandomNode();
      shared_ptr<Node> node = network->getNodeFromUUID(randomUUID);

      clock_unit_t timeToDisconnect = (clock_unit_t)(d1(gen));
      clock_unit_t timeOffline = (clock_unit_t)(d2(gen));

      // CALL FUNCTION TO GIVE PERSCRIBED COMMAND TO node through network
      //This function also garuntees a node cannot go offline more than once at
      //  a time
      network->sendOffline(randomUUID, timeToDisconnect, timeOffline);
    }


}
