#ifndef ADAK_KEYING_NODEDATA_H
#define ADAK_KEYING_NODEDATA_H


// FIXME: make this a tunable parameter
static const double NETWORK_SCALE = 0.3;
static const double ALLOCATION_BEFORE_GIVING_KEYSPACE = 0.7;
static const double CHUNKINESS = 2; // aka. 50%; 1/CHUNKINESS

class Node;

class NodeData {
private:
    int keyShareRate;
    double keyGenRate;
    double aggregateGenRate;
    double shortTermAllocationRatio;
    double longTermAllocationRatio;
    double aggregateAllocationRatio;
    double provisioningRatio;

    double creationRate;

    int keysUsed = 0;
    int day;
    
    // Functions

public:
    int getMinKeyIndex(const std::vector<Keyspace> keyspaces) const;
    ADAK_Key_t findEndKey(double creationRate, std::vector<Keyspace> keyspaces);
    explicit NodeData() : day(getCurrentDay()), keysUsed(0) {}
    ~NodeData() = default;

    static inline bool isNewDay(int currentDay) { return currentDay != getCurrentDay(); }
    static int getCurrentDay();

    inline void useKey() { keysUsed++; 
                           Logger::getConsumption(false);}
    constexpr inline int getKeysUsed() const { return this->keysUsed; }

    double updateCreationRate(const std::map<UUID, std::shared_ptr<Message>> peers);
    
    // Getters and Setters
    constexpr inline double getCreationRate() const {return this->creationRate; }
    double updateLongTermAllocationRatio(std::vector<Keyspace> &keyspace);
    double updateShortTermAllocationRatio(const std::vector<Keyspace> &keyspace);
    double updateProvisioningRatio(double creationRate, double shortTermRatio);
    constexpr inline int getDay() const { return day; }
    
    
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
};


#endif //ADAK_KEYING_NODEDATA_H
