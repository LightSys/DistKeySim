#include <iostream>
#include <ctime>

#include "Simulation.h"
#include "EventGen.h"
#include "Random.h"
using namespace std;

Simulation::Simulation(EventGenerationType eventGenerationType) : network(Network(ConnectionType::Full)), eventGenerationType(eventGenerationType) {
    // Seed random number
    srand(time(nullptr));
    csvOutput = new ofstream();
    csvOutput->open("csvFile.csv", ofstream::out | ofstream::trunc);
}

Simulation::~Simulation() {
    csvOutput->close();
    delete csvOutput;
}

void Simulation::runSimulation() {
    EventGen *eventGenerator;

    if(eventGenerationType == EventGenerationType::Random) {
        eventGenerator = new Random();
    } else if(eventGenerationType == EventGenerationType::Unused1) {
        cout << "This is unimplemented, Baylor team will presumably add their own implementations" << endl;
    }

    // Create root node that will have the max keyspace 0/0
    this->network.addNode();

    // Create new nodes and add them to the map
    for(int i = 0; i < AMOUNT_OF_NODES; i++) {
        this->network.addNode(nullptr);
        this->network.checkAllNodesForMessages();
    }

    // Loop for EventTicks
    for(int i = 0; i < 10; i++) {
        eventGenerator->eventTick(&this->network);
        this->network.checkAllNodesForMessages();
    }

    this->network.printUUIDList();
    this->network.printChannels();
    this->network.printKeyspaces();
    this->network.printUUIDList(this->getCSVOutput());
    this->network.printChannels(this->getCSVOutput());
    this->network.printKeyspaces(this->getCSVOutput());
}