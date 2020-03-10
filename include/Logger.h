#ifndef LOGGER_H
#define LOGGER_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime> 
using namespace std;

const char filename [] = "logOutput.txt";
const char statslog [] = "statslog.csv";
const char numberOfCols = 5;
class Logger {

    public: 
        // static int heartbeat  
        static void deleteOldLog (){
            remove(filename);
            remove(statslog);
        }   
        static void log (string message){
            ofstream myfile;
            myfile.open(filename,ofstream::app);
            time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
            myfile << message << " -- " << ctime(&date)<<endl;
            myfile.close();
        };
        static logStats (int stats []){
            if(stats.size() == numberOfCols){
                std::ofstream myfile;
                myfile.open (statslog);
                for(int i = 0; i < stats.size() ;i++){
                    myfile<<stats[i]<<",";
                }
                myfile << "\n";
                myfile.close();
            }else{
                Logger::log("Stats array was incorrect length");
            }
 
        }

        //stats logger
    };
#endif //LOGGER_H