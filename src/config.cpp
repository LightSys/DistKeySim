#include "config.hpp"

//grabs the data from the jsonFIle and puts it into the variables.
Config::Config(std::ifstream jsonFile) {

    if(jsonFile.is_open()) {
        json jf = json::parse(jsonFile);
        if (jf.contains("numNodes")) {
            jf.at("numNodes").get_to(this->numNodes);
        }
        else {
            numNodes = DEFAULT_NUM_NODES;
        }
        if (jf.contains("connectionMode")){
            jf.at("connectionMode").get_to(this->connectionMode);
        }
        else {
            connectionMode = DEFAULT_CONNECTION_MODE;
        }
        if(jf.contains("csvOutputPath")) {
            jf.at("csvOutputPath").get_to(this->csvOutputPath);
        }
        else{
            csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        }
        if(jf.contains("creationRate")){
            jf.at("creationRate").get_to(this->creationRate);
        }
        else{
            creationRate = DEFAULT_CREATION_RATE;
        }
        if(jf.contains("networkScale")){
            jf.at("networkScale").get_to(this->networkScale);
        }
        else {
            networkScale = DEFAULT_NETWORK_SCALE;
        }
    }
    else {
        numNodes = DEFAULT_NUM_NODES;
        connectionMode = DEFAULT_CSV_OUTPUT_PATH;
        csvOutputPath = DEFAULT_CSV_OUTPUT_PATH;
        creationRate = DEFAULT_CREATION_RATE;
        networkScale = DEFAULT_NETWORK_SCALE;
    }
}

//void from_json(const json& j, gop& g) {
//    j.at("SentenceStressScore").get_to(g.SentenceStressScore);
//    j.at("WordsStressScore").get_to(g.WordsStressScore);
//    j.at("consonant_score").get_to(g.consonant_score);
//    j.at("words_info").get_to(g.words_info);
//}