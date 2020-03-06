#include <fstream>
#include <string>
#include "config.hpp"
#include "json.hpp"
using json = nlohmann::json;

Config Config::fromJSONFile(std::ifstream jsonFile) {
    json jf = json::parse(jsonFile);
    jf.at("AMOUNT_OF_NODES").get_to(jsonFile.AMOUNT_OF_NODES);
    jf.at("fullyConnected").get_to(jsonFile.fullyConnected);
    jf.at("csvOutPutValue").get_to(jsonFile.csvOutPutValue);
    jf.at("creationRate").get_to(jsonFile.creationRate);
    jf.at("networkScale").get_to(jsonFile.networkScale);
}

Config Config::fromJSONString(std::string json) {
    std::ifstream ifs;
    ifs.open("test.json");



}

//void from_json(const json& j, gop& g) {
//    j.at("SentenceStressScore").get_to(g.SentenceStressScore);
//    j.at("WordsStressScore").get_to(g.WordsStressScore);
//    j.at("consonant_score").get_to(g.consonant_score);
//    j.at("words_info").get_to(g.words_info);
//}