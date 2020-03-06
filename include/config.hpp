#ifndef ADAK_KEYING_CONFIG_HPP
#define ADAK_KEYING_CONFIG_HPP

#include <fstream>
#include <string>
#include <utility>

static const u_int DEFAULT_NUM_NODES = 100;
static const char* DEFAULT_CONNECTION_MODE = "full";
static const char* DEFAULT_CSV_OUTPUT_PATH = "out.csv";
static const int DEFAULT_CREATION_RATE = 1;
static const float DEFAULT_NETWORK_SCALE = 0.3;

class Config {
public:
    explicit Config(
        u_int numNodes = DEFAULT_NUM_NODES, std::string connectionMode = DEFAULT_CONNECTION_MODE,
        std::string csvPath = DEFAULT_CSV_OUTPUT_PATH, int creationRate = DEFAULT_CREATION_RATE,
        float networkScale = DEFAULT_NETWORK_SCALE)
           : numNodes(numNodes), connectionMode(std::move(connectionMode)), csvOutputPath(std::move(csvPath)),
             creationRate(creationRate), networkScale(networkScale)
    {}

    Config(std::ifstream jsonFile);

private:
    u_int numNodes;
    std::string connectionMode;
    std::string csvOutputPath;
    int creationRate;
    float networkScale;
};

#endif //ADAK_KEYING_CONFIG_HPP
