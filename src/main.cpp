#include <csignal>
#include <iostream>
#include <string>
#include "getopt.h"

#include "config.hpp"
#include "json.hpp"
#include "Logger.h"
#include "Simulation.h"

using json = nlohmann::json;

using namespace std;

Config config;

void saveLogs()
{
    std::cout << "ADAK saving logs..." << std::endl;
    try
    {
        std::string fullJsonFileName = Logger::copyFile(config.csvOutputPath);
        config.write(fullJsonFileName);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void signalHandler(int signum )
{
    std::cout << "ADAK received interrupt signal " << signum << std::endl;
    saveLogs();
    exit(signum);  
}

int main(int argc, char** argv) {
   // register signal SIGINT and signal handler  
   signal(SIGINT, signalHandler);  

    try {
        std::cout << "ADAK initializing..." << std::endl;
        Logger::deleteOldLog();//clearing old log files
        Logger::setCSVHeaders();//setting the headers of the columns of the csv file
        std::cout << "ADAK configuring..." << std::endl;
        Logger::log(Formatter() << "ADAK loading config from config.json ...");
        config = Config(ifstream("config.json")); 
        try {
            std::cout << "ADAK running simulation..." << std::endl;
            Simulation simulation(config);
            simulation.run();
        } catch (exception& e) {
            Logger::log(Formatter() << e.what());
        }
        saveLogs();
    } catch (exception& e) {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "ADAK is done" << std::endl;
}
