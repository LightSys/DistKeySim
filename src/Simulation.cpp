#include <iostream>

#include "Simulation.h"
#include "UUID.h"
#include "EventGen.h"

Simulation::Simulation()  {
    // Seed random number
    srand(time(NULL));
    this->network = Network(ConnectionType::Full);
}