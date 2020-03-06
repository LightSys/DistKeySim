#ifndef ADAK_KEYING_CONFIG_HPP
#define ADAK_KEYING_CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include "json.hpp"
#include "Simulation.h"

using json = nlohmann::json;

static const u_int DEFAULT_NUM_NODES = 100;
static ConnectionType DEFAULT_CONNECTION_MODE = ConnectionType::Full;
static const char* DEFAULT_CSV_OUTPUT_PATH = "out.csv";
static const int DEFAULT_CREATION_RATE = 1;
static const float DEFAULT_NETWORK_SCALE = 0.3;

struct Config {
    u_int numNodes;
    std::string connModeStr;
    ConnectionType connectionMode;
    std::string csvOutputPath;
    int creationRate;
    float networkScale;
    
    explicit Config(
        u_int numNodes = DEFAULT_NUM_NODES, std::string connectionMode = "full",
        std::string csvPath = DEFAULT_CSV_OUTPUT_PATH, int creationRate = DEFAULT_CREATION_RATE,
        float networkScale = DEFAULT_NETWORK_SCALE)
           : numNodes(numNodes), csvOutputPath(std::move(csvPath)), connModeStr(connectionMode),
             creationRate(creationRate), networkScale(networkScale)
    {
        if (connectionMode == "full") this->connectionMode = ConnectionType::Full;
        else if (connectionMode == "partial") this->connectionMode = ConnectionType::Partial;
        else if (connectionMode == "single") this->connectionMode = ConnectionType::Single;
    
        // Invalid connection mode
        throw std::invalid_argument("connectionMode not one of 'full', 'partial', or 'single'");
    }

    Config(std::ifstream jsonFile);
};

#endif //ADAK_KEYING_CONFIG_HPP
