#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <vector>
#include "message.hpp"
using namespace std;

const vector<string> csvHeadersStatsLog {
    "UUID", "timeSlot", "totalKeys","sharing", "consumption", "remainder", "numKeyspaces"
}; //column headers

const vector<string> csvHeadersKeySharing {
    "UUID", "timeSlot", "shortAlloc", "longAlloc", "prevDay", "prevWeek",
    "avgProv", "avgKey", "provShortRatio", "provLongRatio", "peersChecked"
}; //column headers

class Logger {
    public:
        static void deleteOldLog ();
        static void log (string message);
        static void logStats (vector<string> stats);
        static void logKeySharing (vector<string> keyspace);
        static void logMsg (const string procName, const Message &message);

        static void setCSVHeadersLogStats();
        static void setCSVHeadersKeySharing();
        static int getTimeslot(bool increment);
        static int getShared(bool clear,int more);
        static int getConsumption(bool clear, int more);
    	static std::string copyFile(string path);

        static std::string join(vector<int> ints);
        static std::string join(vector<string> stats, string separator);

        // Removed 2/21/2022 because libunwind.h went missing in GitHub Actions
        // some time between 12/28/2021 and now.
        // static void logBackTrace();

    private:
        static int timeslot;
        static int shared;
        static int rate;
};

// From https://stackoverflow.com/questions/5396790/concatenate-strings-in-function-call-c
class Formatter
{
public:
    template<class Val> Formatter& operator<<(const Val& val)
    {
        ss_ << val;
        return * this;
    }
    operator string () const { return ss_.str().c_str(); }
private:
    std::stringstream ss_;
};
