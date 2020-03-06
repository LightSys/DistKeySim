#include <iostream>
#include <string>
#include "getopt.h"

#include "Simulation.h"
#include "json.hpp"
using json = nlohmann::json;

using namespace std;

int main(int argc, char** argv) {
    // Load config
    Config config(ifstream("config.json"));
    
    Simulation simulation(config);
    simulation.run();
}
