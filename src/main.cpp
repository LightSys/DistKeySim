#include <iostream>
#include <string>
#include "getopt.h"

#include "Simulation.h"

using namespace std;

// Default number of nodes to spin up if no --num argument passed
static const u_int DEFAULT_NUM_NODES = 100;
static const char* DEFAULT_CSV_PATH = "out.csv";

/**
 * Displays the help string for our application
 */
void displayHelp() {
    cout << "Usage: adak [options]\n"
         << "Options:\n"
         << "  -n, --num <val>:       Set number of nodes to test with\n"
         << "  -c, --csv <path>:      Set output CSV path\n"
         << "  -h, --help:            Display this help message"
         << endl;
    exit(1);
}

/**
 *
 * @param argv
 * @param numNodes
 */
void parseArgs(const int &argc, char** argv, u_int &numNodes, string &csvPath) {
    const char* const short_opts = "n:c:h";
    const option long_opts[] = {
        {"num", required_argument, nullptr, 'n'},
        {"csv", required_argument, nullptr, 'c'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };
    
    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1)
    {
        if (opt == -1) {
            break;
        }
        
        switch (opt)
        {
            case 'n':
                numNodes = std::stoi(optarg);
                break;
            
            case 'c':
                csvPath = optarg;
                break;
            
            case 'h': // -h or --help
            case '?': // Unrecognized option
            default:
                displayHelp();
                break;
        }
    }
}

int main(int argc, char** argv) {
    // Process arguments
    u_int numNodes = DEFAULT_NUM_NODES;
    string csvPath = DEFAULT_CSV_PATH;
    parseArgs(argc, argv, numNodes, csvPath);
    
    auto* simulation = new Simulation(EventGenerationType::Random);
    simulation->runSimulation();
}