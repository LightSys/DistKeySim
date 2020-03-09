#ifndef LOGGER_H
#define LOGGER_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime> 
using namespace std;


class Logger {

    public:        
        static void log (string message);
    };
#endif //LOGGER_H