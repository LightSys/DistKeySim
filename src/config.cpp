#include <iostream>

#include "config.hpp"

using namespace std;

Config::Config(ifstream jsonFile) {
    // Check if the JSON file can be opened
    if (jsonFile.good()) {
	cout << "Loading from JSON..." << endl;
        json jf = json::parse(jsonFile);
        
        // This checks for the existence of a key in the JSON file, setting the corresponding key in our Config instance
        // to the value in the file if it exists, otherwise resorting to the default value
        if (jf.contains("numNodes")) {
            jf.at("numNodes").get_to(this->numNodes);
        } else {
            numNodes = DEFAULT_NUM_NODES;
        }
        
        if (jf.contains("connectionMode")) {
            jf.at("connectionMode").get_to(this->connModeStr);
            
            // full, partial, and single are the only implemented connection modes
            if (connModeStr == "full") this->connectionMode = ConnectionType::Full;
            else if (connModeStr == "partial") this->connectionMode = ConnectionType::Partial;
            else if (connModeStr == "single") this->connectionMode = ConnectionType::Single;
            else {
                // Invalid connection mode
                throw std::invalid_argument("connectionMode not one of 'full', 'partial', or 'single'");
            }
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
	cout << "Unable to open JSON file, falling back to defaults..." << endl;
        // Unable to open JSON file, fall back to all default values
        numNodes = DEFAULT_NUM_NODES;
        connModeStr = DEFAULT_CONN_MODE_STR;
        connectionMode = DEFAULT_CONNECTION_MODE;
        csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        creationRate = DEFAULT_CREATION_RATE;
        networkScale = DEFAULT_NETWORK_SCALE;
    }
}
