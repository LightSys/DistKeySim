#include "config.hpp"

using namespace std;

/**
 * Populates Config object from JSON file file stream
 * @param jsonFile File stream to parse JSON from
 */
Config::Config(ifstream jsonFile) {
    if (jsonFile.good()) {
        json jf = json::parse(jsonFile);
        if (jf.contains("numNodes")) {
            jf.at("numNodes").get_to(this->numNodes);
        } else {
            numNodes = DEFAULT_NUM_NODES;
        }
        
        if (jf.contains("connectionMode")) {
            jf.at("connectionMode").get_to(this->connectionMode);
        } else {
            connectionMode = DEFAULT_CONNECTION_MODE;
        }
        
        if (jf.contains("csvOutputPath")) {
            jf.at("csvOutputPath").get_to(this->csvOutputPath);
        } else {
            csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        }
        
        if (jf.contains("creationRate")) {
            jf.at("creationRate").get_to(this->creationRate);
        } else {
            creationRate = DEFAULT_CREATION_RATE;
        }
        
        if (jf.contains("networkScale")) {
            jf.at("networkScale").get_to(this->networkScale);
        } else {
            networkScale = DEFAULT_NETWORK_SCALE;
        }
    } else {
        numNodes = DEFAULT_NUM_NODES;
        connectionMode = DEFAULT_CONNECTION_MODE;
        csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        creationRate = DEFAULT_CREATION_RATE;
        networkScale = DEFAULT_NETWORK_SCALE;
    }
}