#include <iostream>
#include <string>
#include "getopt.h"

#include "json.hpp"
#include "Logger.h"
#include "Simulation.h"

using json = nlohmann::json;

using namespace std;

int main(int argc, char** argv) {
    try {
        std::cout << "ADAK initialization..." << std::endl;
        Logger::deleteOldLog();//clearing old log files
        Logger::setCSVHeaders();//setting the headers of the columns of the csv file
        // Load config
        std::cout << "ADAK configuration..." << std::endl;
        Logger::log(Formatter() << "Attempting to load config from config.json ...");
        Config config(ifstream("config.json")); 
        try {
        std::cout << "ADAK run simulation..." << std::endl;
            Simulation simulation(config);
            simulation.run();
        } catch (exception& e) {
            Logger::log(Formatter() << e.what());
        }
        std::cout << "ADAK save logs..." << std::endl;
        std::string fullJsonFileName = Logger::copyFile(config.csvOutputPath);
        config.write(fullJsonFileName);
    } catch (exception& e) {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "ADAK done" << std::endl;
}
