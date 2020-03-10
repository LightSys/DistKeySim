#ifndef LOGGER_H
#define LOGGER_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime> 
using namespace std;


class Logger {

    public: 
        // static int heartbeat  
        static char [] filename = "logOutput.txt";
        static void deleteOldLog (){
            remove(filename);
        }   
        static void log (string message){
            ofstream myfile;
            myfile.open(filename,ofstream::app);
            time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
            myfile << message << " -- " << ctime(&date)<<endl;
            myfile.close();
        };

        //stats logger
    };
#endif //LOGGER_H