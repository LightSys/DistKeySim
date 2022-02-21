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

const char filename[] = "logOutput.txt";  // general log file
const char statslog[] = "statslog.csv";   // log file for stats output
const char infoMsg[] = "infoMsg.txt";  // Info Msg log file
const char keyMsg[] = "keyMsg.txt";  // Keyspace Msg log file

const vector<string> csvHeaders {
    "UUID", "timeSlot", "totalKeys","sharing", "consumption", "remainder", "numKeyspaces"
}; //column headers

class Logger {
    public:
        static void deleteOldLog ();
        static void log (string message);
        static void logStats (vector<string> stats);
        static void logMsg (const string procName, const Message &message);

        static void setCSVHeaders();
        static int getTimeslot(bool increment);
        static int getShared(bool clear,int more);
        static int getConsumption(bool clear, int more);
    	static std::string copyFile(string path);

        static std::string join(vector<int> ints);
        static std::string join(vector<string> stats, string separator);

        static void logBackTrace();

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
