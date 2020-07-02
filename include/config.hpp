#ifndef ADAK_KEYING_CONFIG_HPP
#define ADAK_KEYING_CONFIG_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include "json.hpp"
#include "Simulation.h"

using json = nlohmann::json;

// Default values when JSON file is missing or missing a key
static const u_int DEFAULT_NUM_NODES = 100;
static ConnectionType DEFAULT_CONNECTION_MODE = ConnectionType::Full;
static const char* DEFAULT_CONN_MODE_STR = "full";
static const char* DEFAULT_CSV_OUTPUT_PATH = "../../outputs/";
static const int DEFAULT_CREATION_RATE = 1;
static const float DEFAULT_NETWORK_SCALE = 0.3;
static const int DEFAULT_LATENCY = 1; //this is the minmum in the current structure right now
static const std::vector<float> DEFAULT_CUSTOM_LAMBDA1 = {};
static const std::vector<float> DEFAULT_CUSTOM_LAMBDA2 = {};
static const std::vector<float> DEFAULT_CUSTOM_LAMBDA3 = {}; //if it is empty, the lambda3 will be used
static const bool DEFAULT_RUN_EVENTS = false; 
static const double DEFAULT_UNITS_PER_DAY = 3600;


//added for UI string inputs:
static const unsigned int SIZEOF_SMALLEST_KEY_OPTIONS = 2;
static const std::string SMALLEST_KEY_OPTIONS[SIZEOF_SMALLEST_KEY_OPTIONS] = {
	"smallest overall",
	"smallest key of the block with the least space left"
};

static const unsigned int SIZEOF_ALGORITHM_STAT_OPTIONS = 2;
static const std::string ALGORITHM_STAT_OPTIONS[SIZEOF_ALGORITHM_STAT_OPTIONS] = {
	"temperature based", 
	"temperature with priority transmission based"
};

static const short INVALID_HARD_KNOB_OPTION = -1;

//defaults for UI input: 
static const float DEFAULT_VISIBLE_PEERS = 0.01;
static const float DEFAULT_LAMBDA1 = 8;
static const float DEFAULT_LAMBDA2 = 6;
static const float DEFAULT_LAMBDA3 = 0.5;
static const unsigned int DEFAULT_MAX_KEYS_BITS = 31;
static const int DEFAULT_CHUNKINESS = 10;
static const int DEFAULT_HEARTBEAT = 5;
static const unsigned int DEFAULT_LONG_PRECISION = 5;
static const unsigned int DEFAULT_SIM_LENGTH = 50;

struct Config {
    //all of the data memebers, in no particular order. 
    //See documentation for purpose of each
    u_int numNodes;
    std::string connModeStr;
    ConnectionType connectionMode;
    std::string csvOutputPath;
    int creationRate;
    float networkScale;
    int latency; 
    bool runEvents;

    //added for the UI input: 
    float visiblePeers;//chance (from percent) in decimal 0-1
    float lambda1;
    float lambda2;
    float lambda3;
    std::vector<float> customLambda1;
    std::vector<float> customLambda2;
    std::vector<float> customLambda3;
    unsigned int maxKeysBits;
    int chunkiness;
    int heartbeatFrequency;
    unsigned int longTermPrecision;
    short smallestKeyOption;
    short algorithmStrategyOption;
    int simLength;
    double unitsPerDay;
    
    //a minimalist intialization of sorts
    explicit Config(
        u_int numNodes = DEFAULT_NUM_NODES, std::string connectionMode = DEFAULT_CONN_MODE_STR,
        std::string csvPath = DEFAULT_CSV_OUTPUT_PATH, int creationRate = DEFAULT_CREATION_RATE,
        float networkScale = DEFAULT_NETWORK_SCALE)
           : numNodes(numNodes), csvOutputPath(std::move(csvPath)), connModeStr(connectionMode),
             creationRate(creationRate), networkScale(networkScale)
    {
        if (connectionMode == "full") this->connectionMode = ConnectionType::Full;
        else if (connectionMode == "partial") this->connectionMode = ConnectionType::Partial;
        else if (connectionMode == "single") this->connectionMode = ConnectionType::Single;
	else if (connectionMode == "custom") this->connectionMode = ConnectionType::Custom;
        else {
            // Invalid connection mode
            throw std::invalid_argument("connectionMode not one of 'full', 'partial', or 'single'");
        }
    }
    
    /**
     * Populates Config object from JSON file file stream
     * @param jsonFile File stream to parse JSON from
     */
    Config(std::ifstream jsonFile);
};

#endif //ADAK_KEYING_CONFIG_HPP
