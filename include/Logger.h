#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
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
            myfile << message << " -- " << ctime(&date); //log message and timestamp
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
	static std::string copyFile(string path){
        // create directory if last char of path is /
        if (path.size() > 0) {
            if (path.back() == '/') {
                filesystem::create_directory(path);
            }
        }

	     int num = 1;
	     //look for the num.txt at the path
	     ifstream numFile(path + "num.txt");
	     if (numFile.is_open()) {
	        numFile >> num;
	        numFile.close();
	     }

	     //increment num.txt file
	     ofstream numFileII(path + "num.txt", std::ofstream::out | std::ofstream::trunc);
	     numFileII << to_string(num + 1);  //replace and increment
	     numFileII.close();
             
	     //copy the statslog
	     ifstream src("./statslog.csv", std::ios::binary);
	     ofstream dest(path + "statslog" + to_string(num) + ".csv", std::ios::binary);   
	     //move over the information
	     dest << src.rdbuf();
	     //close files
	     src.close();
	     dest.close();
             
	     //copy the logoutput.txt
	     src.open("./logOutput.txt", std::ios::binary);
	     dest.open(path + "logOutput" + to_string(num) + ".txt", std::ios::binary);
	     //move over the information
	     dest << src.rdbuf();
	     //close files, and the open next
	     src.close();
	     dest.close();
             
	     //copy the config files
         src.open("config.json", std::ios::binary);
	     dest.open(path + "copy_of_config" + to_string(num) + ".json", std::ios::binary);   
	     //move over the information
	     dest << src.rdbuf();
	     //close files
	     src.close();
	     dest.close();

         return path + "full_config" + to_string(num) + ".json";
	}
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
