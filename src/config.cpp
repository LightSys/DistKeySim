#include <iostream>

#include "config.hpp"

using namespace std;

Config::Config(ifstream jsonFile) {
    // Check if the JSON file can be opened
    if (jsonFile.good()) {
	    Logger::log(Formatter() << "Loading from JSON...");
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
	    else if (connModeStr == "custom") this->connectionMode = ConnectionType::Custom;
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
       
        if (jf.contains("simLength")) {
            jf.at("simLength").get_to(this->simLength);
        } else {
            simLength = DEFAULT_SIM_LENGTH;
        }
         
	if(jf.contains("Latency")){
	    jf.at("Latency").get_to(this->latency);
	} else {
	    latency = DEFAULT_LATENCY;
	}if(jf.contains("runEvents")){
             jf.at("runEvents").get_to(this->runEvents);
	} else{
             runEvents = DEFAULT_RUN_EVENTS;
	}

        if (jf.contains("randomSeed")) {
            jf.at("randomSeed").get_to(this->randomSeed);
	    } else{
            randomSeed = DEFAULT_RANDOM_SEED;
	    }

        //added for the UI input: 
        if (jf.contains("Visible_Peers_(connected_%)")) {
            jf.at("Visible_Peers_(connected_%)").get_to(this->visiblePeers);
            this->visiblePeers /= 100; // percent to decimal chance
        } else {
            this->visiblePeers = DEFAULT_VISIBLE_PEERS;
        }
        
        if (jf.contains("Lambda_1_(time_offline)")) {
            jf.at("Lambda_1_(time_offline)").get_to(this->lambda1);
        } else {
            this->lambda1 = DEFAULT_LAMBDA1;
        }
        
        if (jf.contains("Lambda_2_(time_online)")) {
            jf.at("Lambda_2_(time_online)").get_to(this->lambda2);
        } else {
            this->lambda2 = DEFAULT_LAMBDA2;
        }
        
        if (jf.contains("Max_Keys_(2^n,_give_n)")) {
            jf.at("Max_Keys_(2^n,_give_n)").get_to(this->maxKeysBits);
        } else {
            this->maxKeysBits = DEFAULT_MAX_KEYS_BITS;
        }
        
        if (jf.contains("lambda_3_(time_between_creating_objects)")) {
            jf.at("lambda_3_(time_between_creating_objects)").get_to(this->lambda3);
        } else {
            this->lambda3 = DEFAULT_LAMBDA3;
        }
        
        if(jf.contains("customLambda1")){
           jf.at("customLambda1").get_to(this->customLambda1);
	}else{
           this->customLambda1 = DEFAULT_CUSTOM_LAMBDA1;
	}
        if(jf.contains("customLambda2")){
           jf.at("customLambda2").get_to(this->customLambda2);
	}else{
           this->customLambda2 = DEFAULT_CUSTOM_LAMBDA2;
	}
        if(jf.contains("customLambda3")){
           jf.at("customLambda3").get_to(this->customLambda3);
	}else{
           this->customLambda3 = DEFAULT_CUSTOM_LAMBDA3;
	}

        if (jf.contains("Chunkiness_(#_of_keys_to_shift)")) {
            jf.at("Chunkiness_(#_of_keys_to_shift)").get_to(this->chunkiness);
        } else {
            this->chunkiness = DEFAULT_CHUNKINESS;
        }
        
        if (jf.contains("Heartbeat_Frequency")){
            jf.at("Heartbeat_Frequency").get_to(this->heartbeatFrequency);
        } else {
            this->heartbeatFrequency = DEFAULT_HEARTBEAT;
        }
        
        
        if (jf.contains("Long_Term_Precision")){
            jf.at("Long_Term_Precision").get_to(this->longTermPrecision);
        } else {
            this->longTermPrecision = DEFAULT_LONG_PRECISION;
        }
        
        if (jf.contains("Units_Per_Day")){
            jf.at("Units_Per_Day").get_to(this->unitsPerDay);
        } else {
            this->unitsPerDay = DEFAULT_UNITS_PER_DAY;
        }
        
        //hard knobs: 
        
        if (jf.contains("Smallest_Key_for_Priority")) {
            std::string input;
            jf.at("Smallest_Key_for_Priority").get_to(input);
            bool found = false;
            for(int opt = 0; opt < SIZEOF_SMALLEST_KEY_OPTIONS; opt++){
                if(SMALLEST_KEY_OPTIONS[opt] == input){
                    found = true;
                    this->smallestKeyOption = opt;
                }
            }
            
            if(!found){
                this->smallestKeyOption = INVALID_HARD_KNOB_OPTION;
            }
            
        } else {
            this->smallestKeyOption = INVALID_HARD_KNOB_OPTION;
        }
        
        if (jf.contains("Algorithm_Strategy")) {
            std::string input;
            jf.at("Algorithm_Strategy").get_to(input);
            bool found = false;
            for(int opt = 0; opt < SIZEOF_ALGORITHM_STAT_OPTIONS; opt++){
                if(ALGORITHM_STAT_OPTIONS[opt] == input){
                    found = true;
                    this->algorithmStrategyOption = opt;
                }
            }
            
            if(!found){
                this->algorithmStrategyOption = INVALID_HARD_KNOB_OPTION;
            }
            
        } else {
            this->algorithmStrategyOption = INVALID_HARD_KNOB_OPTION;
        }
        
        
        
    } else {
	Logger::log(Formatter() << "Unable to open JSON file, falling back to defaults...");
        // Unable to open JSON file, fall back to all default values
        numNodes = DEFAULT_NUM_NODES;
        connModeStr = DEFAULT_CONN_MODE_STR;
        connectionMode = DEFAULT_CONNECTION_MODE;
        csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        creationRate = DEFAULT_CREATION_RATE;
        networkScale = DEFAULT_NETWORK_SCALE;
        randomSeed = DEFAULT_RANDOM_SEED;
    }
}
