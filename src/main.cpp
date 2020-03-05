#include <iostream>

#include "Simulation.h"

int main(int argc, char** argv) {
    auto* simulation = new Simulation(EventGenerationType::Random);
    simulation->runSimulation();
}