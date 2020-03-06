#include <fstream>
#include <string>
#include "config.hpp"
#include "json.hpp"
#include <iostream>
using json = nlohmann::json;

Config::Config(std::ifstream jsonFile) {

    json jf = json::parse(jsonFile);
    jf.at("numNodes").get_to(this->numNodes);
    jf.at("connectionMode").get_to(this->connectionMode);
    jf.at("csvOutputPath").get_to(this->csvOutputPath);
    jf.at("creationRate").get_to(this->creationRate);
    jf.at("networkScale").get_to(this->networkScale);
}

//void from_json(const json& j, gop& g) {
//    j.at("SentenceStressScore").get_to(g.SentenceStressScore);
//    j.at("WordsStressScore").get_to(g.WordsStressScore);
//    j.at("consonant_score").get_to(g.consonant_score);
//    j.at("words_info").get_to(g.words_info);
//}