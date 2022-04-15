#include "AbstractStrategy.h"
#include "Logger.h"

void AbstractStrategy::logKeySharing(UUID uuid, double shortAlloc, bool shortAllocIsOne,
        double longAlloc, bool longAllocIsOne, double prevDay, double prevWeek,
        long double avgProv, long double avgKey, int peersChecked) {

    vector<string> dataLine; //line in the csv
    dataLine.push_back(uuid);
    dataLine.push_back(to_string((Logger::getTimeslot(false))));
    dataLine.push_back(to_string(shortAlloc));
    dataLine.push_back(shortAllocIsOne ? "true" : "false");
    dataLine.push_back(to_string(longAlloc));
    dataLine.push_back(longAllocIsOne ? "true" : "false");
    dataLine.push_back(to_string(prevDay));
    dataLine.push_back(to_string(prevWeek));
    dataLine.push_back(to_string(avgProv));
    dataLine.push_back(to_string(avgKey));
    dataLine.push_back(to_string(peersChecked));
    Logger::logKeySharing(dataLine);
}
