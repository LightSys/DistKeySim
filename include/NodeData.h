#ifndef ADAK_KEYING_NODEDATA_H
#define ADAK_KEYING_NODEDATA_H


// FIXME: make this a tunable parameter
static const double NETWORK_SCALE = 0.3;
static const double ALLOCATION_BEFORE_GIVING_KEYSPACE = 0.7;

class Node;

class NodeData {
private:
    double shortTermAllocationRatio;
    double longTermAllocationRatio;

    double creationRate;
    int keysUsed = 0;
    int day;
    
    // Functions
    ADAK_Key_t findEndKey(double creationRate, std::vector<Keyspace> keyspaces);
    int getMinKey(const std::vector<Keyspace> keyspaces) const;
public:
    explicit NodeData() : day(getCurrentDay()), keysUsed(0) {}
    ~NodeData() = default;

    static inline bool isNewDay(int currentDay) { return currentDay != getCurrentDay(); }
    static int getCurrentDay();

    inline void useKey() { keysUsed++; }
    constexpr inline int getKeysUsed() const { return this->keysUsed; }

    double updateCreationRate(const std::map<UUID, std::shared_ptr<Message>> peers);

    // Getters and Setters
    constexpr inline double getCreationRate() const {return this->creationRate; }
    double updateLongTermAllocationRatio(const std::vector<Keyspace> &keyspace);
    double updateShortTermAllocationRatio(const std::vector<Keyspace> &keyspace, u_int minSpaceIdx,
                                          const std::map<UUID, std::shared_ptr<Message>> &peers);
    
//    int getKeyShareRate() const { return keyShareRate; }
//    void setKeyShareRate(int keyShareRate) { NodeData::keyShareRate = keyShareRate; }
//
//    double getKeyGenRate() const { return keyGenRate; }
//    void setKeyGenRate(double keyGenRate) { NodeData::keyGenRate = keyGenRate; }
//
//    double getAggregateGenRate();
//    void setAggregateGenRate(double aggregateGenRate) { NodeData::aggregateGenRate = aggregateGenRate; }
//
//    double getShortTermAllocationRatio() const { return shortTermAllocationRatio; }
//    void setShortTermAllocationRatio(double shortTermAllocationRatio) { NodeData::shortTermAllocationRatio = shortTermAllocationRatio; }
//
//    double getLongTermAllocationRatio() const { return longTermAllocationRatio; }
//    void setLongTermAllocationRatio(double longTermAllocationRatio) { NodeData::longTermAllocationRatio = longTermAllocationRatio; }
//
//    double getAggregateAllocationRatio() const { return aggregateAllocationRatio; }
//    void setAggregateAllocationRatio(double aggregateAllocationRatio) { NodeData::aggregateAllocationRatio = aggregateAllocationRatio; }
//
//    double getProvisioningRatio() const { return provisioningRatio; }
//    void setProvisioningRatio(double provisioningRatio) { NodeData::provisioningRatio = provisioningRatio; }

    constexpr inline int getDay() const { return day; }
};


#endif //ADAK_KEYING_NODEDATA_H
