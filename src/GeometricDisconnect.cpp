/**
 * implementation of the geometric disconnect class
 */

#include "SystemClock.h"
#include "GeometricDisconnect.h"

GeometricDisconnect::GeometricDisconnect(ClockType clockType, double lambda1, double lambda2){
    clock = shared_ptr<SystemClock>(SystemClock::makeClock(clockType));

    this->lambda1 = lambda1;
    this->lambda2 = lambda2;

    // d1 is the model used to randomly generate the amount of time from the
    // current time step when the node will go offline
    d1 = new geometric_distribution<>(1/lambda1);
    // d2 is the model used to randomly generate the amount of time the current
    // node will remain offline
    d2 = new geometric_distribution<>(1/lambda2);

    // These are required for the geometric distribution function to operate
    // correctly
    random_device rd;
    // Save the seed for debugging
    unsigned int seed = rd();
    gen = new mt19937(seed);

}

void GeometricDisconnect::sendOffline(UUID nodeUUID, clock_unit_t timeToDisconnect, clock_unit_t timeOffline){
    //only allow a node to have one offline period set at a time
    if(backOnlineTimer.find(nodeUUID) == backOnlineTimer.end()){

        //need to start the timer until the offline starts
        //need to record the length of the offline
        //once the timer ends, need to reset the timer with offline length
        //  and record that it's offline (somehow)

        //set up when it goes offline
        goOfflineTimer[nodeUUID] = nextTimerID;
        clock->setTimer(nextTimerID, timeToDisconnect);
        nextTimerID++;

        //set up when if comes back online after going offline
        backOnlineTimer[nodeUUID] = nextTimerID;
        clock->setTimer(nextTimerID, timeToDisconnect + timeOffline);
        nextTimerID++;
        if (Logger::logOutputVerbose) Logger::log(Formatter() << "just prepped " << nodeUUID << " to go offline"); 
    }
}

void GeometricDisconnect::checkOffline(Network* network){
    vector<UUID> toClear;

    //detect if a node's offline timer has expired (meaning that it is time to go offline)
    for(auto i : goOfflineTimer){
        if(clock->checkTimer(i.second) == TimerStatus::ENDED){
            toClear.push_back(i.first);
            clock->clearTimer(i.second);
            network->disableNode(i.first);
            if (Logger::logOutputVerbose) Logger::log(Formatter() << "just send  " << i.first << " offline");
        }
    }

    //clear the timers which have expired
    for(UUID i : toClear){
        goOfflineTimer.erase(goOfflineTimer.find(i));
    }
    toClear.clear();

    //detect if a node's online timer has expired (meaning that it's time to come back online)
    for(auto i : backOnlineTimer){
        if(clock->checkTimer(i.second) == TimerStatus::ENDED){
            toClear.push_back(i.first);
            clock->clearTimer(i.second);
            network->enableNode(i.first);
            if (Logger::logOutputVerbose) Logger::log(Formatter() << "just woke up " << i.first); 
	}
    }

    //clear the timers which have expired
    for(UUID i : toClear){
        //goOfflineTimer.erase(goOfflineTimer.find(i)); //this is the wrong vector
	backOnlineTimer.erase(backOnlineTimer.find(i));
    }
    toClear.clear();
}

void GeometricDisconnect::eventTick(Network* network) {
    //check the offline timers and update node status's as necessary
    this->checkOffline(network);

//FIXME: make this based on a percentage of numNodes. 
    // random number between 1-10
    //srand (time(NULL));
    //int totalNodes = rand() % 10 + 1;
    
    map<UUID, std::shared_ptr<Node>> nodes = network->getNodes(); 
    for (auto i : nodes){
        UUID nodeUUID = i.first;
        shared_ptr<Node> node = i.second;
        clock_unit_t timeToDisconnect = (clock_unit_t)(node->getTimeOffline());  //Cannot do imediatly; no 0.
        clock_unit_t timeOffline = (clock_unit_t)(node->getTimeOnline());        //No instant timers; no 0 

        // CALL FUNCTION TO GIVE PERSCRIBED COMMAND TO node through network
        //This function also guarantees a node cannot go offline more than once at
        //  a time
        if (Logger::logOutputVerbose) Logger::log(Formatter() << "attempting to send node " << nodeUUID
            << " offline in " << timeToDisconnect << " for " << timeOffline);
        this->sendOffline(nodeUUID, timeToDisconnect, timeOffline);
    }

     
    //tick the internal clock
    clock->tick();  
}
