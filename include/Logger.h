#ifndef LOGGER_H
#define LOGGER_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <vector>
using namespace std;

const char filename [] = "logOutput.txt";
const char statslog [] = "statslog.csv";
const char numberOfCols = 6;
const vector<string> csvHeaders { "UUID", "timeSlot", "totalKeys","sharing", "consumption", "remainder" }; 
class Logger {
    private:
        static int timeSlot =0;
    public:
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
        }
        static void logStats (vector<string> stats){
            if(stats.size() == numberOfCols){
                ofstream myfile;
                myfile.open (statslog, ofstream::app);
                for(int i = 0; i < stats.size() ;i++){
                    myfile<<stats[i]<<",";
                }
                myfile << "\n";
                myfile.close();
            }else{
                Logger::log("Stats array was incorrect length");
            }
        }

        static void setCSVHeaders (){
            Logger::logStats(csvHeaders);
        }
        static void incrementTimeslot (){
            timeSlot++;
        }    
    };
#endif //LOGGER_H
