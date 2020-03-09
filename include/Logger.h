#ifndef LOGGER_H
#define LOGGER_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime> 
using namespace std;


class Logger {

    public:        
        static void log (string message){
            string fileName = "logOutput.txt";
            ofstream myfile;
            myfile.open(fileName,ofstream::app);
            time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
            myfile << message << " -- " << ctime(&date)<<endl;
            myfile.close();
        };
    };
#endif //LOGGER_H