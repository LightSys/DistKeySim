#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <climits>
#include <map> 

#include "UUID.h"
#include "Keyspace.h"
#include "message.hpp"
#include "NodeData.h"
#include "Logger.h"

class NodeData;

static const HexDigest BROADCAST_UUID = "00000000-0000-0000-0000-000000000000";


class Node {
private:
    UUID uuid;
    u_int messageID = 1;
    std::deque<Message> sendQueue;
    std::vector<Keyspace> keyspaces;
    std::map<UUID, std::pair<Message*, uint64_t>> peers; //Hold the message and the next ID to send
    //map to store confirmation messages
    std::map<uint64_t, Message*> confirmMsg;  
    
    int currentTick;
    std::map<uint64_t, int> timeStamps; //stores when each message was sent`
    int networkLatency; 
    
    // Node statistics
    NodeData lastDay;
    float createdDay;
    float createdWeek;
    ADAK_Key_t totalLocalKeyspaceSize=0;
    double objectConsumptionRatePerSecond = 0;
    double amountOfOneKeyUsed = 0;
    double lambda3;
    double lambda2;
    double lambda1;
    long double keysShared; 
    
    // d3 is the model used to randomly generate the object consuption rate
    geometric_distribution<> *d3;
    geometric_distribution<> *d1;
    geometric_distribution<> *d2;

    mt19937 *gen;   
    /**
     * Generates the heartbeat informational message instance as per the specification
     * @param peerID UUID of peer to send to
     * @return Message with hearbeat information for this node
     */
    Message getHeartbeatMessage(const UUID &peerID);
    double networkScale; 

    //helper functions
    uint64_t getNextMsgId(UUID peerId);
    
    
public:
    //static
    //handle ticking of simulation
    Node();     
    Node(double lambda1, double lambda2, double lambda3, int latency, double netScl, unsigned seed);
    Node(const Keyspace &keyspace, double lambda1, double lambda2, double lambda3, int latency, double netScl, unsigned seed);
    static Node rootNode(double lambda1, double lambda2, double lambda3, int latency, double networkScale, unsigned seed);
    ~Node(){
        delete d3;
	delete d2;
	delete d1;
        delete gen;
	//delete message pointers in confirmMsg 
    }
    
    //returns the sum of all the keyspace percentages
    double getKeyspacePercent(); 
    
    //the sum of all of the keyspace sizes in keys
    unsigned long long int getTotalKeyspaceBlockSize(); 

    /**
     * Adds a peer to local list of peers
     * @param peer Const reference to Node instance to add as peer
     */
    void addPeer(std::shared_ptr<Node> peer);

    /**
     * Adds a peer to local list of peers
     * @param peer Reference to UUID of peer to add
     */
    void addPeer(const UUID &peerUUID);

    /**
     * Attempts to remove peer based on pointer
     * @param peer Reference to Node instance
     */
    void removePeer(std::shared_ptr<Node> peer);

    /**
     * Attempts to remove peer based on UUID
     * @param peerUUID UUID of peer to attempt to remove
     */
    void removePeer(const UUID &peerUUID);

    // Network interaction
    /**
     * Moves all messages out of queue
     * @return Vector of all messages queued for sending
     */
    std::deque<Message> getMessages();
    std::deque<Message> checkMessages(); //same as getMessages, but do not delete 

    // Generate heartbeat messages for all peers
    void heartbeat();

    //update the current tick
    void tick(); 

    //return success
    bool receiveMessage(const Message &message);

    /**
     * Change the consumption rate according the geometric distribution
     */
    void changeConsumptionRate();

    /**
     * This generates the geometric distribution from which the consumption
     * rate is randomly generated with changeConsumptionRate
     */
    void generateObjectCreationRateDistribution(unsigned seed);

    //returns the smallest keyspace larger than newMin (with no input it returns the smallest overall)
    int minimumKeyspaceIndex(int newMin = -1);
    
    //consumes the smallest key the Node has
    ADAK_Key_t getNextKey();
    
    //check if the node has any valid keyspaces that have unconsumed keys
    bool isKeyAvailable();

    // consume objects as determined by the rate of consumption
    void consumeObjects();
    
    //getter/setter
    UUID getUUID() const { return uuid; }
    void setUUID(UUID nid) { uuid = nid; }
   
    std::vector<Keyspace> getKeySpace() const { return keyspaces; }
     
    const std::map<UUID, pair<Message*, uint64_t>> getPeers() const { return this->peers; }

    /**
     * Retrieves latest statistics for this node
     * @return Safe shared pointer to most recent NodeData instance
     */
    std::shared_ptr<NodeData> getNodeData() const;

    void setTotalLocalKeyspaceSize(ADAK_Key_t newSize) {this->totalLocalKeyspaceSize = newSize;};

    ADAK_Key_t getTotalLocalKeyspaceSize() const {return this->totalLocalKeyspaceSize;};

    //adds the information to the logger about this node/
    void logInfoForHeartbeat();
    
    //split the designated keyspace in half
    int splitKeyspace(int keyspaceIndex);

    //send the specifed keyspace to the peer with the specifed index
    void sendCustKeyspace(UUID id, int keyInd);

    //send multiple keyspaces
    void sendCustKeyspace(UUID id, vector<int> keyInds);

    //check if the last keyspace message sent to the peer was confirmed
    bool canSendKeyspace(UUID id);

    //make a sub block if of the designated size 
    vector<int> makeSubBlock(int range);

    //getters
    double getCreatedDay(){return createdDay;}
    double getCreatedWeek(){return createdWeek;} 
    
    //calculate the long and short aggegaetes
    double calcLongAggregate(UUID target);
    double calcShortAggregate(UUID target);

    //retunr the value from d2
    double getTimeOnline();

    //return the value from d1
    double getTimeOffline();    
};