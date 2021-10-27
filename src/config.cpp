#include <iostream>

#include "config.hpp"
#include "ConnectionType.h"

using namespace std;

const std::string Config::ALGORITHM_STRATEGY_LABEL = "Algorithm_Strategy";
const std::string Config::CHUNKINESS_LABEL = "Chunkiness_(#_of_keys_to_shift)";
const std::string Config::CONNECTION_MODE_LABEL = "connectionMode";
const std::string Config::CREATION_RATE_LABEL = "Creation_Rate(%_of_keyspace)";
const std::string Config::CSV_OUTPUT_PATH_LABEL = "csvOutputPath";
const std::string Config::CUSTOM_CONNECTIONS_LABEL = "Custom_Connections";
const std::string Config::CUSTOM_LAMBDA_1_LABEL = "customLambda1";
const std::string Config::CUSTOM_LAMBDA_2_LABEL = "customLambda2";
const std::string Config::CUSTOM_LAMBDA_3_LABEL = "customLambda3";
const std::string Config::HEARTBEAT_FREQUENCY_LABEL = "Heartbeat_Frequency";
const std::string Config::LAMBDA_1_LABEL = "Lambda_1_(time_offline)";
const std::string Config::LAMBDA_2_LABEL = "Lambda_2_(time_online)";
const std::string Config::LAMBDA_3_LABEL = "lambda_3_(time_between_creating_objects)";
const std::string Config::LATENCY_LABEL = "Latency";
const std::string Config::LONG_TERM_PRECISION_LABEL = "Long_Term_Precision";
const std::string Config::MAX_KEYS_LABEL = "Max_Keys_(2^n,_give_n)";
const std::string Config::NETWORK_SCALE_LABEL = "networkScale";
const std::string Config::NUM_NODES_LABEL = "numNodes";
const std::string Config::RANDOM_SEED_LABEL = "randomSeed";
const std::string Config::RUN_EVENTS_LABEL = "runEvents";
const std::string Config::SIM_LENGTH_LABEL = "simLength";
const std::string Config::SMALLEST_KEY_FOR_PRIORITY_LABEL = "Smallest_Key_for_Priority";
const std::string Config::TIME_STEP_UNITS_PER_SECOND_LABEL = "Time_Step_Units_Per_Second";
const std::string Config::VISIBLE_PEERS_LABEL = "Visible_Peers_(connected_%)";

unsigned Config::timeStepUnitsPerSecond = 1;
unsigned Config::timeStepUnitsPerMinute = Config::timeStepUnitsPerSecond * 60;
unsigned Config::timeStepUnitsPerHour   = Config::timeStepUnitsPerMinute * 60;
unsigned Config::timeStepUnitsPerDay    = Config::timeStepUnitsPerHour * 24;
unsigned Config::timeStepUnitsPerWeek   = Config::timeStepUnitsPerDay * 7;  

Config::Config(ifstream jsonFile) {
    // Check if the JSON file can be opened
    if (jsonFile.good()) {
	    Logger::log(Formatter() << "Loading from JSON...");
        json jf = json::parse(jsonFile);
        
        // This checks for the existence of a key in the JSON file, setting the corresponding key in our Config instance
        // to the value in the file if it exists, otherwise resorting to the default value
        if (jf.contains(NUM_NODES_LABEL)) {
            jf.at(NUM_NODES_LABEL).get_to(this->numNodes);
        } else {
            numNodes = DEFAULT_NUM_NODES;
        }
        
        if (jf.contains(CONNECTION_MODE_LABEL)) {
            jf.at(CONNECTION_MODE_LABEL).get_to(this->connModeStr);
            
            // full, partial, and single are the only implemented connection modes
            this->connectionMode = ConnectionType_fromString(connModeStr);
            if (this->connectionMode == ConnectionType::Custom) {
                // Invalid connection mode
                throw std::invalid_argument("connectionMode not one of 'full', 'partial', or 'single'");
            }
        } else {
            connectionMode = DEFAULT_CONNECTION_MODE;
        }
        
        if (jf.contains(CSV_OUTPUT_PATH_LABEL)) {
            jf.at(CSV_OUTPUT_PATH_LABEL).get_to(this->csvOutputPath);
        } else {
            csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        }
        
        if (jf.contains(CREATION_RATE_LABEL)) {
            jf.at(CREATION_RATE_LABEL).get_to(this->creationRate);
        } else {
            creationRate = DEFAULT_CREATION_RATE;
        }
        
        if (jf.contains(NETWORK_SCALE_LABEL)) {
            jf.at(NETWORK_SCALE_LABEL).get_to(this->networkScale);
        } else {
            networkScale = DEFAULT_NETWORK_SCALE;
        }
       
        if (jf.contains(SIM_LENGTH_LABEL)) {
            jf.at(SIM_LENGTH_LABEL).get_to(this->simLength);
        } else {
            simLength = DEFAULT_SIM_LENGTH;
        }
         
        if(jf.contains(LATENCY_LABEL)){
            jf.at(LATENCY_LABEL).get_to(this->latency);
        } else {
            latency = DEFAULT_LATENCY;
        }
        
        if(jf.contains(RUN_EVENTS_LABEL)){
                jf.at(RUN_EVENTS_LABEL).get_to(this->runEvents);
        } else{
                runEvents = DEFAULT_RUN_EVENTS;
        }

        if (jf.contains(RANDOM_SEED_LABEL)) {
            jf.at(RANDOM_SEED_LABEL).get_to(this->randomSeed);
	    } else{
            randomSeed = DEFAULT_RANDOM_SEED;
	    }

        if (jf.contains(CUSTOM_CONNECTIONS_LABEL)) {
            jf.at(CUSTOM_CONNECTIONS_LABEL).get_to(this->customConnections);
        } else {
            customConnections = DEFAULT_CUSTOM_CONNECTIONS;
        }
        
        //added for the UI input: 
        if (jf.contains(VISIBLE_PEERS_LABEL)) {
            jf.at(VISIBLE_PEERS_LABEL).get_to(this->visiblePeers);
            this->visiblePeers /= 100; // percent to decimal chance
        } else {
            this->visiblePeers = DEFAULT_VISIBLE_PEERS;
        }
        
        if (jf.contains(LAMBDA_1_LABEL)) {
            jf.at(LAMBDA_1_LABEL).get_to(this->lambda1);
        } else {
            this->lambda1 = DEFAULT_LAMBDA1;
        }
        
        if (jf.contains(LAMBDA_2_LABEL)) {
            jf.at(LAMBDA_2_LABEL).get_to(this->lambda2);
        } else {
            this->lambda2 = DEFAULT_LAMBDA2;
        }
        
        if (jf.contains(MAX_KEYS_LABEL)) {
            jf.at(MAX_KEYS_LABEL).get_to(this->maxKeysBits);
        } else {
            this->maxKeysBits = DEFAULT_MAX_KEYS_BITS;
        }
        
        if (jf.contains(LAMBDA_3_LABEL)) {
            jf.at(LAMBDA_3_LABEL).get_to(this->lambda3);
        } else {
            this->lambda3 = DEFAULT_LAMBDA3;
        }
        
        if(jf.contains(CUSTOM_LAMBDA_1_LABEL)){
           jf.at(CUSTOM_LAMBDA_1_LABEL).get_to(this->customLambda1);
    	}else{
           this->customLambda1 = DEFAULT_CUSTOM_LAMBDA1;
	    }
        if(jf.contains(CUSTOM_LAMBDA_2_LABEL)){
           jf.at(CUSTOM_LAMBDA_2_LABEL).get_to(this->customLambda2);
	    }else{
           this->customLambda2 = DEFAULT_CUSTOM_LAMBDA2;
	    }
        if(jf.contains(CUSTOM_LAMBDA_3_LABEL)){
           jf.at(CUSTOM_LAMBDA_3_LABEL).get_to(this->customLambda3);
	    }else{
           this->customLambda3 = DEFAULT_CUSTOM_LAMBDA3;
	    }

        if (jf.contains(CHUNKINESS_LABEL)) {
            jf.at(CHUNKINESS_LABEL).get_to(this->chunkiness);
        } else {
            this->chunkiness = DEFAULT_CHUNKINESS;
        }
        
        if (jf.contains(HEARTBEAT_FREQUENCY_LABEL)){
            jf.at(HEARTBEAT_FREQUENCY_LABEL).get_to(this->heartbeatFrequency);
        } else {
            this->heartbeatFrequency = DEFAULT_HEARTBEAT;
        }
        
        if (jf.contains(LONG_TERM_PRECISION_LABEL)){
            jf.at(LONG_TERM_PRECISION_LABEL).get_to(this->longTermPrecision);
        } else {
            this->longTermPrecision = DEFAULT_LONG_PRECISION;
        }
        
        if (jf.contains(TIME_STEP_UNITS_PER_SECOND_LABEL)){
            jf.at(TIME_STEP_UNITS_PER_SECOND_LABEL).get_to(timeStepUnitsPerSecond);
        } else {
            timeStepUnitsPerSecond = DEFAULT_TIME_STEP_UNITS_PER_SECOND;
        }
        timeStepUnitsPerMinute = timeStepUnitsPerSecond * 60;
        timeStepUnitsPerHour = timeStepUnitsPerMinute * 60;
        timeStepUnitsPerDay = timeStepUnitsPerHour * 24;
        timeStepUnitsPerWeek = timeStepUnitsPerDay * 7;
    	Logger::log(Formatter() << "timeStepUnitsPerSecond is "<< timeStepUnitsPerSecond);
    	Logger::log(Formatter() << "timeStepUnitsPerMinute is "<< timeStepUnitsPerMinute);
    	Logger::log(Formatter() << "timeStepUnitsPerHour is "<< timeStepUnitsPerHour);
    	Logger::log(Formatter() << "timeStepUnitsPerDay is "<< timeStepUnitsPerDay);
    	Logger::log(Formatter() << "timeStepUnitsPerWeek is "<< timeStepUnitsPerWeek);
        
        //hard knobs: 
        
        if (jf.contains(SMALLEST_KEY_FOR_PRIORITY_LABEL)) {
            std::string input;
            jf.at(SMALLEST_KEY_FOR_PRIORITY_LABEL).get_to(input);
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
        
        if (jf.contains(ALGORITHM_STRATEGY_LABEL)) {
            std::string input;
            jf.at(ALGORITHM_STRATEGY_LABEL).get_to(input);
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
        customConnections = DEFAULT_CUSTOM_CONNECTIONS;
    }
}

void Config::write(std::string jsonFile) {
    json j;

    j[ALGORITHM_STRATEGY_LABEL] = ALGORITHM_STAT_OPTIONS[this->algorithmStrategyOption];
    j[CHUNKINESS_LABEL] = this->chunkiness;
    j[CONNECTION_MODE_LABEL] = ConnectionType_toString(this->connectionMode);
    j[CREATION_RATE_LABEL] = this->creationRate;
    j[CSV_OUTPUT_PATH_LABEL] = this->csvOutputPath;
    j[CUSTOM_CONNECTIONS_LABEL] = this->customConnections;
    if (this->customLambda1.size() > 0) {j[CUSTOM_LAMBDA_1_LABEL] = this->customLambda1;}
    if (this->customLambda2.size() > 0) {j[CUSTOM_LAMBDA_2_LABEL] = this->customLambda2;}
    if (this->customLambda3.size() > 0) {j[CUSTOM_LAMBDA_3_LABEL] = this->customLambda3;}
    j[HEARTBEAT_FREQUENCY_LABEL] = this->heartbeatFrequency;
    j[LAMBDA_1_LABEL] = this->lambda1;
    j[LAMBDA_2_LABEL] = this->lambda2;
    j[LAMBDA_3_LABEL] = this->lambda3;
    j[LATENCY_LABEL] = this->latency;
    j[LONG_TERM_PRECISION_LABEL] = this->longTermPrecision;
    j[MAX_KEYS_LABEL] = this->maxKeysBits;
    j[NETWORK_SCALE_LABEL] = this->networkScale;
    j[NUM_NODES_LABEL] = this->numNodes;
    j[RANDOM_SEED_LABEL] = this->randomSeed;
    j[SIM_LENGTH_LABEL] = this->simLength;
    j[SMALLEST_KEY_FOR_PRIORITY_LABEL] = SMALLEST_KEY_OPTIONS[this->smallestKeyOption];
    j[VISIBLE_PEERS_LABEL] = this->visiblePeers * 100;
    
    std::ofstream o(jsonFile);
    o << std::setw(4) << j << std::endl;
}
