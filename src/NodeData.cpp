#include <iostream>
#include <ctime>

#include "Node.h"
#include "NodeData.h"

using namespace std;

NodeData::NodeData(Node* parentNode) : parentNode(parentNode) {
    day = getCurrentDay();
}


bool NodeData::isNewDay(int currentDay) {
    return currentDay != getCurrentDay();
}

int NodeData::getCurrentDay() {
    // current date/time based on current system
    time_t now = time(0);

//    cout << "Number of sec since January 1,1970:" << now << endl;

    tm *ltm = localtime(&now);

//    // print various components of tm structure.
//    cout << "Year" << 1900 + ltm->tm_year << endl;
//    cout << "Month: "<< 1 + ltm->tm_mon<< endl;
//    cout << "Day: "<<  ltm->tm_mday << endl;
//    cout << "Time: "<< 1 + ltm->tm_hour << ":";
//    cout << 1 + ltm->tm_min << ":";
//    cout << 1 + ltm->tm_sec << endl;
    return ltm->tm_mday;
}

double NodeData::updateCreationRate() {
    double totalRate = 0.0;
    for(Node* node : parentNode->getPeers()) {
        totalRate += node->getNodeData()->getKeysUsed();
    }
    totalRate *= NETWORK_SCALE;
    creationRate = this->getKeysUsed() + totalRate;
}

double NodeData::updateLongTermAllocationRatio(){
    double longTermRatio =0.0;
    for(Keyspace* keyspace: parentNode->getKeySpace()){
        longTermRatio +=  (double)(1.0 / (pow(2, keyspace->getSuffix())));
    }
    longTermAllocationRatio = longTermRatio;
    return longTermRatio;
}

double NodeData::updateShortTermAllocationRatio(){
    double shortTermRatio = 0.0;
    int tempKeys =keysUsed;
    if(keysUsed = 0){
        tempKeys = 1;
    }
    shortTermRatio = keysUsed/();
}

