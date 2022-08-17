#pragma once

#include "Logger.h"
#include "Damping.h"

static const double NETWORK_SCALE = 0.3;
static const double ALLOCATION_BEFORE_GIVING_KEYSPACE = 0.7;
static const double CHUNKINESS = 2; // aka. 50%; 1/CHUNKINESS

class Node;

class NodeData {
private:
    int keyShareRate = 0;
    double keyGenRate = 0;
    double aggregateGenRate = 0;
    Damping shortTermAllocationRatio;
    double longTermAllocationRatio = 0;
    double aggregateAllocationRatio = 0;
    double provisioningRatio = 0;

    double creationRate = 0;

    int keysUsed = 0;
    int day = 0;
    
    int timeUnitsPast = 0;
    
    // Functions

public:
     explicit NodeData() : day(getCurrentDay()), keysUsed(0) {}
    ~NodeData() = default;

    //outdated functions for the current day, based on onll ticking system
    static inline bool isNewDay(int currentDay) { return currentDay != getCurrentDay(); }
    static int getCurrentDay();
    
    //incriments stats and logs it
    inline void useKey() { keysUsed++; 
                           Logger::getConsumption(false,1);}
    

    // Getters and Setters
    constexpr inline int getKeysUsed() const { return this->keysUsed; }
    constexpr inline double getCreationRate() const {return this->creationRate; }
    double updateLongTermAllocationRatio(std::vector<Keyspace> &keyspace);
    double updateShortTermAllocationRatio(const std::vector<Keyspace> &keyspace);
    double updateProvisioningRatio(double creationRate, double shortTermRatio);
    constexpr inline int getDay() const { return day; }
    constexpr inline int getTimeUnitsPast() const {return this->timeUnitsPast; }
    int getKeyShareRate() const;
    void setKeyShareRate(int keyShareRate);
    double getKeyGenRate() const;
    void setKeyGenRate(double keyGenRate);
    double getAggregateGenRate() const;
    void setAggregateGenRate(double aggregateGenRate);
    double getShortTermAllocationRatio() const;
    void setShortTermAllocationRatio(double shortTermAllocationRatio);
    double getLongTermAllocationRatio() const;
    void setLongTermAllocationRatio(double longTermAllocationRatio);
    double getAggregateAllocationRatio() const;
    void setAggregateAllocationRatio(double aggregateAllocationRatio);
    double getProvisioningRatio() const;
    void setProvisioningRatio(double provisioningRatio);
    void setCreationRate(double creationRate);
    void setKeysUsed(int keysUsed);
    void setDay(int day);

    //returns the index of the keysapce with the lowest keu value
    int getMinKeyIndex(const std::vector<Keyspace> keyspaces) const;

    //a faulty function. No longer used because a) the largest key is either UINT_MAX or they only have sub-block, 
    //which should never happend, and b) it just returns the same value as getMinKey (hence the aforementioned "faulty")
    ADAK_Key_t findEndKey(double creationRate, std::vector<Keyspace> keyspaces);
       
    //incriments how many ticks this node data has been in use
    void incTimeUnitPast() {timeUnitsPast++;} 

    string toString() {
        return Formatter() << "dayInMonth=" << day << " keysUsed=" << keysUsed << " timeUnitsPast=" << timeUnitsPast
            << " shortAlloc=" << shortTermAllocationRatio.getValue()
            << " longAlloc=" << longTermAllocationRatio;

    }
};
