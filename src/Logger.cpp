#include <numeric>

#include "Logger.h"

static ofstream logOutputStream;
static ofstream logStatsStream;

void Logger::deleteOldLog() {
    remove(filename);
    remove(statslog);
    logOutputStream.open(filename, ofstream::app);
    logStatsStream.open(statslog, ofstream::app);
}

void Logger::log(string message) {
    time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
    logOutputStream << message << " -- " << ctime(&date);  // log message and timestamp
    logOutputStream.flush();
}

void Logger::logStats(vector<string> stats) {
    if (stats.size() == csvHeaders.size()) {
        for (int i = 0; i < stats.size(); i++) {  // write each line to csv
            logStatsStream << stats[i] << ",";
        }
        logStatsStream << "\n";
    } else {  // don't log if array isn't accurate size
        log("Stats array was incorrect length");
    }
}

void Logger::setCSVHeaders() {
    logStats(csvHeaders);  // first line should be column titles
}

int Logger::getTimeslot(bool increment) {
    int timeslot = 0;
    if (increment) {  // only increment if it is from doAllHearbeat()
        timeslot++;
    }
    return timeslot;
}

int Logger::getShared(bool clear, int more) {
    int shared = 0;
    shared += more;
    if (clear) {  // reset after timeslot changes
        shared = 0;
    }
    return shared;
}

int Logger::getConsumption(bool clear, int more) {
    int rate = 0;
    rate += more;
    if (clear) {  // reset after timeslot changes
        rate = 0;
    }
    return rate;
}

std::string Logger::copyFile(string path) {
    logOutputStream.close();
    logStatsStream.close();
    // create directory if last char of path is /
    if (path.size() > 0) {
        if (path.back() == '/') {
            filesystem::create_directory(path);
        }
    }

    int num = 1;
    // look for the num.txt at the path
    ifstream numFile(path + "num.txt");
    if (numFile.is_open()) {
        numFile >> num;
        numFile.close();
    }

    // increment num.txt file
    ofstream numFileII(path + "num.txt", std::ofstream::out | std::ofstream::trunc);
    numFileII << to_string(num + 1);  // replace and increment
    numFileII.close();

    // copy the statslog
    ifstream src("./statslog.csv", std::ios::binary);
    ofstream dest(path + "statslog" + to_string(num) + ".csv", std::ios::binary);
    // move over the information
    dest << src.rdbuf();
    // close files
    src.close();
    dest.close();

    // copy the logoutput.txt
    src.open("./logOutput.txt", std::ios::binary);
    dest.open(path + "logOutput" + to_string(num) + ".txt", std::ios::binary);
    // move over the information
    dest << src.rdbuf();
    // close files, and the open next
    src.close();
    dest.close();

    // copy the config files
    src.open("config.json", std::ios::binary);
    dest.open(path + "copy_of_config" + to_string(num) + ".json", std::ios::binary);
    // move over the information
    dest << src.rdbuf();
    // close files
    src.close();
    dest.close();

    return path + "full_config" + to_string(num) + ".json";
}

std::string Logger::join(vector<int> ints) {
    std::cout << "join: " << std::flush;
    for (int i=0; i < ints.size(); i++)
        std::cout << ints.at(i) << ' ' << std::flush;
    std::cout << std::endl << std::flush;
    std::cout << "join: done" << std::endl << std::flush;

    return std::accumulate(ints.begin()+1, ints.end(), std::to_string(ints[0]),
        [](const std::string& a, int b){
            return a + ',' + std::to_string(b);
        });
}
