#include <iostream>
#include <ctime>

#include "Simulation.h"

Simulation::Simulation() : network(Network(ConnectionType::Full)) {
    // Seed random number
    srand(time(NULL));
}