#include <iostream>
#include <ctime>

#include "Simulation.h"

using namespace std;

Simulation::Simulation() : network(Network(ConnectionType::Full)) {
    // Seed random number
    srand(time(nullptr));
    csvOutput = new ofstream();
    csvOutput->open("csvFile.csv", std::ofstream::out | std::ofstream::trunc);
}

Simulation::~Simulation() {
    csvOutput->close();
    delete csvOutput;
}