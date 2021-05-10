#include <iostream>
#include <string>
#include "getopt.h"

#include "json.hpp"
#include "Logger.h"
#include "Simulation.h"

using json = nlohmann::json;

using namespace std;

int main(int argc, char** argv) {
    // Load config
    Logger::log(Formatter() << "Attempting to load config from config.json ...");
    Config config(ifstream("config.json")); 
    Simulation simulation(config);
    Logger::deleteOldLog();//clearing old log files
    Logger::setCSVHeaders();//setting the headers of the columns of the csv file
    simulation.run();
    std::string fullJsonFileName = Logger::copyFile(config.csvOutputPath);
    config.write(fullJsonFileName);
}
