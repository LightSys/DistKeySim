#include <fstream>
#include <string>

#include "config.hpp"

Config Config::fromJSONFile(std::ifstream jsonFile) {}

Config Config::fromJSONString(std::string json) {}

//void from_json(const json& j, gop& g) {
//    j.at("SentenceStressScore").get_to(g.SentenceStressScore);
//    j.at("WordsStressScore").get_to(g.WordsStressScore);
//    j.at("consonant_score").get_to(g.consonant_score);
//    j.at("words_info").get_to(g.words_info);
//}