#include <fstream>
#include <string>
#include "config.hpp"
#include "json.hpp"
#include <iostream>
using json = nlohmann::json;

Config::Config(std::ifstream jsonFile) {

    if(jsonFile.is_open()) {
        json jf = json::parse(jsonFile);
        if (jf.contains("numNodes")) {
            jf.at("numNodes").get_to(this->numNodes);
        }
        if (jf.contains("connectionMode")){
            jf.at("connectionMode").get_to(this->connectionMode);
        }
        if(jf.contains("csvOutputPath")){
            jf.at("csvOutputPath").get_to(this->csvOutputPath);
        }
        if(jf.contains("creationRate")){
            jf.at("creationRate").get_to(this->creationRate);
        }
        if(jf.contains("networkScale")){
            jf.at("networkScale").get_to(this->networkScale);
        }
    }
}

//void from_json(const json& j, gop& g) {
//    j.at("SentenceStressScore").get_to(g.SentenceStressScore);
//    j.at("WordsStressScore").get_to(g.WordsStressScore);
//    j.at("consonant_score").get_to(g.consonant_score);
//    j.at("words_info").get_to(g.words_info);
//}