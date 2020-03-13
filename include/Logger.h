#ifndef LOGGER_H
#define LOGGER_H


#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <vector>
using namespace std;

const char filename [] = "logOutput.txt"; //general log file
const char statslog [] = "statslog.csv";//log file for stats output
const vector<string> csvHeaders { "UUID", "timeSlot", "totalKeys","sharing", "consumption", "remainder" }; //column headers
class Logger {
    public:
        static void deleteOldLog (){
            remove(filename);
            remove(statslog);
        }
        static void log (string message){
            ofstream myfile;
            myfile.open(filename,ofstream::app);
            time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
            myfile << message << " -- " << ctime(&date)<<endl; //log message and timestamp
            myfile.close();
        }
        static void logStats (vector<string> stats){
            if(stats.size() == csvHeaders.size()){
                ofstream myfile;
                myfile.open (statslog, ofstream::app);
                for(int i = 0; i < stats.size() ;i++){//write each line to csv
                    myfile<<stats[i]<<",";
                }
                myfile << "\n";
                myfile.close();
            }else{//don't log if array isn't accurate size
                Logger::log("Stats array was incorrect length");
            }
        }

        static void setCSVHeaders (){
            Logger::logStats(csvHeaders);//first line should be column titles
        }
        static int getTimeslot(bool increment){
            static int timeslot = 0;
            if (increment){//only increment if it is from doAllHearbeat()
                timeslot++;
            }
            return timeslot;
        }  
        static int getShared(bool clear,int more){
            static int shared = 0;
            shared += more;
            if(clear){//reset after timeslot changes
                shared =0;
            }
            return shared;
        }  
        static int getConsumption(bool clear, int more){
            static int rate = 0;
            rate+=more;
            if(clear){//reset after timeslot changes
                rate =0;
            }
            return rate;
        }
    };
#endif //LOGGER_H
