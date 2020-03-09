#include "Logger.h"

static void log (string message){
    string fileName = "logOutput.txt";
    ofstream myfile;
    myfile.open(fileName);
    time_t date = chrono::system_clock::to_time_t(chrono::system_clock::now());
    myfile << message << " -- " << ctime(&date)<<endl;
    myfile.close();
}