#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <vector>
using namespace std;

const char filename[] = "logOutput.txt";  // general log file
const char statslog[] = "statslog.csv";   // log file for stats output

const vector<string> csvHeaders {
    "UUID", "timeSlot", "totalKeys","sharing", "consumption", "remainder", "numKeyspaces"
}; //column headers

class Logger {
    public:
        static void deleteOldLog ();
        static void log (string message);
        static void logStats (vector<string> stats);

        static void setCSVHeaders();
        static int getTimeslot(bool increment);
        static int getShared(bool clear,int more);
        static int getConsumption(bool clear, int more);
    	static std::string copyFile(string path);
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
