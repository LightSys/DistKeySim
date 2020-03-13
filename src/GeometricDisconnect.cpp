#include <ctime>
#include <random>

#include "GeometricDisconnect.h"

using namespace std;

GeometricDisconnect(ClockType clockType, double lambda1, double lambda2){
    clock = shared_ptr<SystemClock>(SystemClock.makeClock(clockType));

    this->lambda1 = lambda1;
    this->lambda2 = lambda2;

    // d1 is the model used to randomly generate the amount of time from the
    // current time step when the node will go offline
    d1 = new geometric_distribution<>(lambda1);
    // d2 is the model used to randomly generate the amount of time the current
    // node will remain offline
    d2 = new geometric_distribution<>(lambda2);

    // These are required for the geometric distribution function to operate
    // correctly
    random_device rd;
    // Save the seed for debugging
    unsigned int seed = rd();
    gen = new mt19937(seed);
}

void GeometricDisconnect::sendOffline(Network* network, UUID nodeUUID, clock_unit_t timeToDisconnect, clock_unit_t timeOffline){
    //only allow a node to have one offline period set at a time
    if(backOnlineTimer.find(nodeUUID) == backOnlineTimer.end()){

        //need to start the timer until the offline starts
        //need to record the length of the offline
        //once the timer ends, need to reset the timer with offline length
        //  and record that it's offline (somehow)

        //set up when it goes offline
        goOfflineTimer[nodeUUID] = nextTimerID;
        clock.setTimer(nextTimerID, timeToDisconnect);
        nextTimerID++;

        //set up when if comes back online after going offline
        backOnlineTimer[nodeUUID] = nextTimerID;
        clock.setTimer(nextTimerID, timeToDisconnect + timeOffline);
        nextTimerID++;
    }
}

void GeometricDisconnect::checkOffline(Network* network){
    vector<UUID> toClear;

    //detect if a node's offline timer has expired (meaning that it is time to go offline)
    for(auto i : goOfflineTimer){
        if(clock.checkTimer(i.second) == TimerStatus::ENDED){
            toClear.push_back(i.first);
            clock.clearTimer(i.second);
            network->disableNode(i.first);
        }
    }

    //clear the timers which have expired
    for(UUID i : toClear){
        goOfflineTimer.erase(goOfflineTimer.find(i));
    }
    toClear.clear();

    //detect if a node's online timer has expired (meaning that it's time to come back online)
    for(auto i : backOnlineTimer){
        if(clock.checkTimer(i.second) == TimerStatus::ENDED){
            toClear.push_back(i.first);
            clock.clearTimer(i.second);
            network->enableNode(i.first);
        }
    }

    //clear the timers which have expired
    for(UUID i : toClear){
        goOfflineTimer.erase(goOfflineTimer.find(i));
    }
    toClear.clear();
}

void GeometricDisconnect::eventTick(Network* network) {
    //check the offline timers and update node status's as necessary
    this->checkOffline(network);


    // random number between 1-10
    int totalNodes = rand() % 10 + 1;

    for (int i = 0; i < totalNodes; i ++){
      UUID randomUUID = network->getRandomNode();
      shared_ptr<Node> node = network->getNodeFromUUID(randomUUID);

      clock_unit_t timeToDisconnect = (clock_unit_t)((*d1)(*gen));
      clock_unit_t timeOffline = (clock_unit_t)((*d2)(*gen));

      // CALL FUNCTION TO GIVE PERSCRIBED COMMAND TO node through network
      //This function also garuntees a node cannot go offline more than once at
      //  a time
      network->sendOffline(randomUUID, timeToDisconnect, timeOffline);
    }

    //tell nodes to CONSUMEEEEE (nom nom nom nom nom)
    network->tellAllNodesToConsumeObjects();

    //TODO:::: randomly change object consuption rate for a random node
}
