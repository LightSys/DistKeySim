#include <iostream>

#include "Logger.h"
#include "Network.h"
#include "Node.h"
#include "UUID.h"
#include "ControlStrategy.h"

#include <chrono>

using namespace std;

Network::Network(ConnectionType connectionType, float PERCENT_CONNECTED, double lambda1, double lambda2, double lambda3, double netScale, int latency, bool enableSendMsgLog)
	: lambda1(lambda1), lambda2(lambda2), lambda3(lambda3), networkScale(netScale), latency(latency),
      enableSendMsgLog(enableSendMsgLog) {
    this->connectionType = connectionType;
    //PERCENT_CONNECTED is a 5 digit int (99.999% = 99999)
    this->PERCENT_CONNECTED = (int)(PERCENT_CONNECTED*1000);
    Logger::log("Network online");
    custLambda3 = {};
    custLambda2 = {};
    custLambda1 = {};
}

Network::Network(ConnectionType connectionType, float PERCENT_CONNECTED, double lambda1, double lambda2, 
		double lambda3, double netScale,
	       	int latency, vector<float>lam1s, vector<float>lam2s, vector<float>lam3s, bool enableSendMsgLog)
	:  lambda1(lambda1), lambda2(lambda2), lambda3(lambda3), networkScale(netScale), latency(latency),
       enableSendMsgLog(enableSendMsgLog) {
    this->connectionType = connectionType;
    //PERCENT_CONNECTED is a 5 digit int (99.999% = 99999)
    this->PERCENT_CONNECTED = (int)(PERCENT_CONNECTED*1000);
    Logger::log("Network online");
    custLambda3 = lam3s;
    custLambda2 = lam2s;
    custLambda1 = lam1s;
}



void Network::tellAllNodesToConsumeObjects(){
    // To make this repeatable, loop through nodes in order they were added
    for (int i=0; i<uuids.size(); i++) {
        auto uuid = uuids.at(i);
        auto node = nodes.at(uuid);
        node->consumeObjects();
    }
}

//sends a single node offline
void Network::disableNode(UUID nodeUUID){
    nodeStatus[nodeUUID] = false;
    string message = nodeUUID + " has gone offline.";
    Logger::log(message);//log that the node has gone offline
}

//sends a single node online
void Network::enableNode(UUID nodeUUID){
    nodeStatus[nodeUUID] = true;
}

bool Network::isOffline(UUID nodeID){
    return !nodeStatus[nodeID];
}

bool Network::sendMsg(const Message &message) {
    //Logger::logBackTrace();
    UUID destID = message.destnodeid(), srcID = message.sourcenodeid();

    if (!isOffline(destID) && !isOffline(srcID)) {
        if (enableSendMsgLog) {
            Logger::logMsg(Formatter() << srcID << " sendMsg", message);
        }
        shared_ptr<Node> destNode = getNodeFromUUID(destID);
        return destNode->receiveMessage(message);
    } else {
        return false;
    }
}

void Network::doAllHeartbeat(AbstractStrategy *adakStrategy, int keysToSend) {
    // NOTE: Baylor will need to change this, right now we are sending heartbeat messages practically all the time
    // they will probably want to add time to Node to send it periodically
    // To make this repeatable, loop through nodes in order they were added
    for (int i=0; i<uuids.size(); i++) {
        auto uuid = uuids.at(i);
        auto node = nodes.at(uuid);
//auto start = std::chrono::high_resolution_clock::now();

    	node->heartbeat();
//auto end = std::chrono::high_resolution_clock::now();
//Logger::log("sending a heartbeat took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//      	<< " ns");

//start = std::chrono::high_resolution_clock::now();
        Logger::log(Formatter() << "doAllHeartbeat: uuid=" << uuid);
        adakStrategy->adak(*(node), keysToSend); //have the node decide what to do 
//end = std::chrono::high_resolution_clock::now();
//Logger::log("chekcing control strat took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//       	<< " ns");    
    }
 
}

void Network::doAllTicks(){
    // To make this repeatable, loop through nodes in order they were added
    for (int i=0; i<uuids.size(); i++) {
        auto uuid = uuids.at(i);
        auto node = nodes.at(uuid);
    	node->tick();
    }
}

void Network::checkAndSendAllNodes() {
    for (auto const& node : nodes) {
        // Check each node for queued messages, moving all messages to here before working
        deque<Message> nodeMsgs = node.second->getMessages();
        bool gaveKeyspace = false;
        for (auto const &msg : nodeMsgs) {
            if (gaveKeyspace && (msg.messagetype() == Message_MessageType_KEYSPACE)) {
                // Gave keyspace, don't give any more
                continue;
            }

            gaveKeyspace |= sendMsg(msg);
        }
    }
}

void Network::checkAndSendAllNodesLatency(int latency) {
    //Wait for latencyStall ticks to send anything  
    vector<Message> tickMsgs;
  
    for (auto const& node : nodes) {    
	// Check each node for queued messages, moving all messages to here before working
        deque<Message> nodeMsgs = node.second->getMessages();
        bool gaveKeyspace = false;

	for (auto const &msg : nodeMsgs) {
	    tickMsgs.push_back(msg);
        }
    }
    //store them to send later
    toSend.push(tickMsgs);

    if(latency > latencyStall){
       latencyStall++; //essetially, do nothing
    }else{
        vector<Message> temp = toSend.front();
	    toSend.pop(); //get rid of timestep
	    for(int i = 0  ; i < temp.size() ; i ++){
            //auto start = std::chrono::high_resolution_clock::now();
		    sendMsg(temp[i]);
            //auto end = std::chrono::high_resolution_clock::now();
            //Logger::log("sending a message took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ms"); 
	    }
    }
}

shared_ptr<Node> Network::getNodeFromUUID(const UUID &uuid) const {
    return nodes.find(uuid)->second;
}

UUID Network::addRootNode(unsigned seed) {
    UUID root = addNode(Keyspace(0, UINT_MAX, 0), seed);
    nodeStatus[root] = true;
    return root;
}

UUID Network::addEmptyNode(unsigned seed) {
    // Create a new new node with the no keyspace
    // if consumption rates (lambda3) are custom, use that. Otherwise, use the provided
    
    double tempLam1, tempLam2, tempLam3;
    if(!custLambda1.empty()){
       tempLam1 = custLambda1.front();
       custLambda1.erase(custLambda1.begin());

    }else{
       tempLam1 = this->lambda1;
    }

    if(!custLambda2.empty()){
       tempLam2 = custLambda2.front();
       custLambda2.erase(custLambda2.begin());

    }else{
       tempLam2 = this->lambda2;
    }

    if(!custLambda3.empty()){
       tempLam3 = custLambda3.front();
       custLambda3.erase(custLambda3.begin());

    }else{
       tempLam3 = this->lambda3;
    }

    shared_ptr<Node> newNode = make_shared<Node>(tempLam1, tempLam2, tempLam3, latency, networkScale, seed);


    UUID newUUID = newNode->getUUID();

    // Add the new node to the nodes map
    nodes.insert({newNode->getUUID(), newNode});
    uuids.push_back(newUUID);

    // Make connection to peers
    connectNodeToNetwork(newNode);

    nodeStatus[newNode->getUUID()] = true;

    return newUUID;
}

UUID Network::addNode(const Keyspace &keyspace, unsigned seed) {
    // Create a new new node with the given keyspace
        double tempLam1, tempLam2, tempLam3;
    if(!custLambda1.empty()){
       tempLam1 = custLambda1.front();
       custLambda1.erase(custLambda1.begin());

    }else{
       tempLam1 = this->lambda1;
    }

    if(!custLambda2.empty()){
       tempLam2 = custLambda2.front();
       custLambda2.erase(custLambda2.begin());

    }else{
       tempLam2 = this->lambda2;
    }

    if(!custLambda3.empty()){
       tempLam3 = custLambda3.front();
       custLambda3.erase(custLambda3.begin());

    }else{
       tempLam3 = this->lambda3;
    }

    shared_ptr<Node> newNode = make_shared<Node>(keyspace, tempLam1, tempLam2, tempLam3, latency, networkScale, seed); 
    connectNodeToNetwork(newNode);
    UUID newUUID = newNode->getUUID();

    // Add the new node to the nodes map
    nodeStatus[newNode->getUUID()] = true;
    nodes.insert({newNode->getUUID(), std::move(newNode)});
    uuids.push_back(newUUID);

    //log node created
    return newUUID;
}

void Network::connectNodeToNetwork(shared_ptr<Node> newNode) {
    if (this->connectionType == ConnectionType::Full) {
        fullyConnect(newNode);
    } else if (this->connectionType == ConnectionType::Partial) {
        // rand() % 4 is an arbitrary number to make the connection only happen sometimes.
        //PERCENT_CONNECTED is a 5 digit int (99.9995% = 99999)
        int coinFlip = rand() % PERCENT_CONNECTED;
        for (auto &[uuid, node] : nodes) {
            // Don't connect the node to itself
            if (newNode->getUUID() == node->getUUID()) {
                continue;
            }

            // Make sure that the channel doesn't already exist
            if (!channelExists(node->getUUID(), node->getUUID()) && coinFlip == 0) {
                connectNodes(node->getUUID(), node->getUUID());
            }
        }

        singleConnect(newNode);
    } else if (this->connectionType == ConnectionType::Single) {//randomly generated MST
        singleConnect(newNode);
    } else if (this->connectionType == ConnectionType::Custom){
	customConnect(newNode);
    }

}

void Network::fullyConnect(shared_ptr<Node> node) {
    for (auto const &[uuid, peer] : nodes) {
        // Don't connect the node to itself
        if (node->getUUID() == uuid) {
            continue;
        }

        // Make sure that the channel doesn't already exist
        if (channelExists(node->getUUID(), uuid)) {
            continue;
        }

        // No connection, create one
        connectNodes(node->getUUID(), uuid);
    }
}

void Network::singleConnect(shared_ptr<Node> node) {
    // Go through each node and make sure it has at least 1 connection
    if (nodes.size() > 1) {
        UUID randomUUID = getRandomNode();

        // On the off chance that getRandomNode() picks the current node, then we need another node
        while (randomUUID == node->getUUID()) {
            randomUUID = getRandomNode();
        }
        connectNodes(node->getUUID(), randomUUID);
    }
}

//only truly makes connections once the final node is added. Returns isDone. 
bool Network::customConnect(shared_ptr<Node> node) {   
    //connect to json file... 
    std::ifstream inFile ("config.json", std::ifstream::binary);
    json in = json::parse(inFile); //in is the json object
    //other vars
    int numNodes; 
    string connections;
    //check if map of nodes is the same length as numnodes:
    in.at("numNodes").get_to(numNodes);

    //only make matches once all nodes are made
    if(numNodes == nodes.size()){
	//get the connections form the json file
	in.at("Custom_Connections").get_to(connections);
        map<string, UUID> strToID;
        
	//make each connection
        std::map<UUID, std::shared_ptr<Node>>::iterator nextNode = nodes.begin(); //which node to use next
	while(connections.find("[") != string::npos){
	   //connections in the form [a, b]  means a connects to b.
           //parse pairs
	   int start = connections.find("[");
	   int comma = connections.find(",");
	   int end = connections.find("]");
           string node1 = connections.substr(start + 1, comma - start - 1);
	   string node2 = connections.substr(comma + 2, end - comma - 2); 
	   //cut off current pair from connections
	   connections = connections.substr(end +1); 
       //Logger::log(Formatter() << "connecting Nodes " << node1 << " and " << node2);
	   //Assign nodes to connection values if needed
	   if(strToID.find((node1)) == strToID.end()){
	    if(nextNode == nodes.end()){
	        //Logger::log(Formatter() << "Cannot have a node to represent connection with node " << node1);
		    continue; //cannot finish this iteration, nothing to connect
	     }else{ 
                strToID.insert(pair<string, UUID>(node1, nextNode->first));
	        nextNode++;
	     }
	   } 
	   //and now the second item
	   if(strToID.find(node2) == strToID.end()){
	     if(nextNode == nodes.end()){
	        //Logger::log(Formatter() << "Cannot have a node to represent connection with node " << node2);
		    continue; //cannot finish this loop; nothing to connect
	     }else{
	       strToID.insert(pair<string, UUID>(node2, nextNode->first));
	       nextNode++;
	     }
	   } 



	   //connect the pair together
	   connectNodes(strToID[node1], strToID[node2]);

        }
	//finshed making connections so return true
	return true;
    }
    
    //false; is not done making connections
    return false;

}



bool Network::channelExists(const UUID &nodeOne, const UUID &nodeTwo) {
    return getChannelIndex(nodeOne, nodeTwo) != -1;
}

int Network::getChannelIndex(const UUID &nodeOne, const UUID &nodeTwo) {
    auto channelItr = find_if(
        channels.begin(),
        channels.end(),
        [nodeOne, nodeTwo](Channel &channel) {
            return (
                (channel.getToNode() == nodeOne && channel.getFromNode() == nodeTwo) ||
                (channel.getToNode() == nodeTwo && channel.getFromNode() == nodeOne)
            );
        }
    );

    return (channelItr != channels.end()) ? distance(channels.begin(), channelItr) : -1;
}

void Network::connectNodes(const UUID &nodeOne, const UUID &nodeTwo) {
    // If trying to connect the same node or trying to create duplicate connection
    if (nodeOne == nodeTwo || channelExists(nodeOne, nodeTwo)) {
        return;
    }

    Channel channel(nodeOne, nodeTwo);
    channels.push_back(channel);

    shared_ptr<Node> node1 = getNodeFromUUID(nodeOne);
    shared_ptr<Node> node2 = getNodeFromUUID(nodeTwo);
    node1->addPeer(node2);
    node2->addPeer(node1);
}

void Network::disconnectNodes(const UUID &nodeOne, const UUID &nodeTwo) {
    int channelIndex = getChannelIndex(nodeOne, nodeTwo);
    if (channelIndex > -1) {
        const Channel &channel = channels.at(channelIndex);

        // Remove peer from node
        shared_ptr<Node> node1 = getNodeFromUUID(channel.getFromNode());
        shared_ptr<Node> node2 = getNodeFromUUID(channel.getToNode());
        node1->removePeer(node2->getUUID());
        node2->removePeer(node1->getUUID());

        channels.erase(channels.begin() + channelIndex);
    }
}

vector<UUID> Network::generateUUIDList() {
    vector<UUID> uuidList;

    // Loop through array and get all the UUIDs
    for (auto const& x : nodes) {
        uuidList.push_back((UUID) x.first);
    }
    return uuidList;
}

UUID Network::getRandomNode() {
    vector<UUID> uuidList = generateUUIDList();

    // Select a random value from the new UUID list
    int randomNum = rand() % uuidList.size();
    return uuidList.at(randomNum);
}

void Network::printUUIDList(char spacer) {
    int counter = 0;
    Logger::log(Formatter() << "UUID List");
    Logger::log(Formatter() << "COUNT" << spacer << "UUID (in hex)" << spacer << "# bits");
    for (UUID const uuid : generateUUIDList()) {
        Logger::log(Formatter() << counter << spacer << uuid << spacer << (uuid.size() * 8) << spacer);
        counter++;
    }
}
void Network::printChannels(char spacer) {
    Logger::log(Formatter() << "CHANNELS");
    Logger::log(Formatter() << "TO" << spacer << "FROM" << spacer << "ID");
    for (const Channel &channel : channels) {
        Logger::log(Formatter() << channel.getToNode() << spacer
            << channel.getFromNode() << spacer
            << channel.getChannelId()
            << (this->isOffline(channel.getToNode()) || this->isOffline(channel.getFromNode())
                ? "OFFLINE!!" : ""));
    }
    Logger::log(Formatter() << "END CHANNELS");
}

void Network::printKeyspaces(char spacer) {
    Logger::log(Formatter() << "KEYSPACES");
    Logger::log(Formatter()
        << "Node Count" << spacer
        << "UUID" << spacer
        << "Start" << spacer
        << "End" << spacer
        << "Suffix Bits" << spacer
        << "Keyspace Count");

    // To make this repeatable, loop through nodes in order they were added
    int nodeCounter = 0;
    int keyspaceCounter = 0;
    for (int i=0; i<uuids.size(); i++) {
        auto uuid = uuids.at(i);
        auto node = nodes.at(uuid);
        for (const Keyspace &keyspace : node->getKeySpace()) {
            Logger::log(Formatter()
                << nodeCounter << spacer
                << node->getUUID() << spacer
                << keyspace.getStart() << spacer
                << keyspace.getEnd() << spacer
                << keyspace.getSuffix() << spacer
                << keyspaceCounter++
            );
        }
        nodeCounter++;
    }
    Logger::log(Formatter() << "END KEYSPACES");
}

double Network::checkAllKeyspace(){
    double total = 0;
    //sum all keyspaces hold
    for (auto const& x : nodes) {
        total += x.second->getKeyspacePercent();

	//look at node's tosend deque
	deque<Message> temp = x.second->checkMessages();
        for (auto const &msg : temp) {
	    if(msg.messagetype() == Message_MessageType_KEYSPACE){
               int msgSize = msg.keyspace().keyspaces_size(); 
	       for(int k =  0; k < msgSize; k++) {
		  if(msg.keyspace().keyspaces(k).endid() < UINT_MAX) continue; 
                  total += 1.0/pow(2, msg.keyspace().keyspaces(k).suffixbits());
	       }
	    } 
        }
    }
    
    //sum all in messages to be sent
    int size = toSend.size(); 
    for(int i = 0 ; i < size ; i ++){
        vector<Message> temp = toSend.front();
	toSend.pop(); //remove the first item
	toSend.push(temp); //place it back in the queue. Will loop around to original pos.
	int tempSize = temp.size();
	for(int j = 0 ; j < tempSize ; j ++){
            if(temp[j].messagetype() == Message_MessageType_KEYSPACE){
               int msgSize =temp[j].keyspace().keyspaces_size();  
	       for(int k =  0; k < msgSize; k++) {
                  total += 1.0/pow(2, temp[j].keyspace().keyspaces(k).suffixbits());
	       }
	    } 
	}
    }
    Logger::log(Formatter() << "total keyspace is " << total * 100 << "% of the keyspace");
    return total; 
}
