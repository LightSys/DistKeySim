#include <iostream>

#include "Network.h"
#include "Node.h"
#include "UUID.h"
#include "ControlStrategy.h"

#include <chrono>

using namespace std;

Network::Network(ConnectionType connectionType, float PERCENT_CONNECTED, double lambda1, double lambda2, double lambda3, double netScale, int latency)
	: lambda1(lambda1), lambda2(lambda2), lambda3(lambda3), networkScale(netScale), latency(latency) {
    this->connectionType = connectionType;
    //PERCENT_CONNECTED is a 5 digit int (99.999% = 99999)
    this->PERCENT_CONNECTED = (int)(PERCENT_CONNECTED*1000);
    cout << "Network online" << endl;
    custLambda3 = {};
    custLambda2 = {};
    custLambda1 = {};
}

Network::Network(ConnectionType connectionType, float PERCENT_CONNECTED, double lambda1, double lambda2, 
		double lambda3, double netScale,
	       	int latency, vector<float>lam1s, vector<float>lam2s, vector<float>lam3s)
	:  lambda1(lambda1), lambda2(lambda2), lambda3(lambda3), networkScale(netScale), latency(latency) {
    this->connectionType = connectionType;
    //PERCENT_CONNECTED is a 5 digit int (99.999% = 99999)
    this->PERCENT_CONNECTED = (int)(PERCENT_CONNECTED*1000);
    cout << "Network online" << endl;
    custLambda3 = lam3s;
    custLambda2 = lam2s;
    custLambda1 = lam1s;
}



void Network::tellAllNodesToConsumeObjects(){
    for(auto i : nodes){
        i.second->consumeObjects();
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
    UUID destID = message.destnodeid(), srcID = message.sourcenodeid();

    if(!isOffline(destID) && !isOffline(srcID)){
        shared_ptr<Node> destNode = getNodeFromUUID(destID);
        return destNode->receiveMessage(message);
    } else {
        return false;
    }
}

void Network::doAllHeartbeat(int keysToSend) {
    // NOTE: Baylor will need to change this, right now we are sending heartbeat messages practically all the time
    // they will probably want to add time to Node to send it periodically
    for (auto const& node : nodes) {
//auto start = std::chrono::high_resolution_clock::now();

    	node.second->heartbeat();
//auto end = std::chrono::high_resolution_clock::now();
//cout << "sending a heartbeat took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//      	<< " ns" << endl;

//start = std::chrono::high_resolution_clock::now();
      	ControlStrategy::adak(*(node.second), keysToSend); //have the node decide what to do 
//end = std::chrono::high_resolution_clock::now();
//cout << "chekcing control strat took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//       	<< " ns" << endl;    
    }
 
}

void Network::doAllTicks(){
    for (auto const& node : nodes) {
	node.second->tick();
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
//cout << "sending a message took " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
//       	<< " ms" << endl; 
	}
    }
}

shared_ptr<Node> Network::getNodeFromUUID(const UUID &uuid) const {
    return nodes.find(uuid)->second;
}

UUID Network::addRootNode() {
    UUID root = addNode(Keyspace(0, UINT_MAX, 0));
    nodeStatus[root] = true;
    return root;
}

UUID Network::addEmptyNode() {
    // Create a new new node with the no keyspace
    // if consumtion rates (lambda3) are custom, use that. Otherwise, use the provided
    
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

    shared_ptr<Node> newNode = make_shared<Node>(tempLam1, tempLam2, tempLam3, latency, networkScale);


    UUID newUUID = newNode->getUUID();

    // Add the new node to the nodes map
    nodes.insert({newNode->getUUID(), newNode});

    // Make connection to peers
    connectNodeToNetwork(newNode);

    nodeStatus[newNode->getUUID()] = true;

    return newUUID;
}

UUID Network::addNode(const Keyspace &keyspace) {
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

    shared_ptr<Node> newNode = make_shared<Node>(keyspace, tempLam1, tempLam2, tempLam3, latency, networkScale); 
    connectNodeToNetwork(newNode);
    UUID newUUID = newNode->getUUID();

    // Add the new node to the nodes map
    nodeStatus[newNode->getUUID()] = true;
    nodes.insert({newNode->getUUID(), move(newNode)});

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
           cout << "connecting Nodes " << node1 << " and " << node2 << endl; 
	   //Assign nodes to connection values if needed
	   if(strToID.find((node1)) == strToID.end()){
	    if(nextNode == nodes.end()){
	        cout << "Cannot have a node to represent connection with node " << node1 << endl;
		continue; //cannot finish this iteration, nothing to connect
	     }else{ 
                strToID.insert(pair<string, UUID>(node1, nextNode->first));
	        nextNode++;
	     }
	   } 
	   //and now the second item
	   if(strToID.find(node2) == strToID.end()){
	     if(nextNode == nodes.end()){
	        cout << "Cannot have a node to represent connection with node " << node2 << endl;
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

void Network::printUUIDList() {
    printUUIDList(cout, ' ');
}

void Network::printUUIDList(ostream &out, char spacer) {
    int counter = 0;
    out << "UUID List" << '\n'
        << "COUNT" << spacer << "UUID (in hex)" << spacer << "# bits" << '\n';
    for (UUID const uuid : generateUUIDList()) {
        out << counter << spacer << uuid << spacer << (uuid.size() * 8) << spacer << '\n';
        counter++;
    }
    out << flush;
}
void Network::printChannels() {
    printChannels(cout, ' ');
}

void Network::printChannels(ostream &out, char spacer) {
    out << "CHANNELS\n"
        << "TO" << spacer << "FROM" << spacer << "ID" << "\n";
    for (const Channel &channel : channels) {
        out << channel.getToNode() << spacer
            << channel.getFromNode() << spacer
            << channel.getChannelId()
            << endl;
        if(this->isOffline(channel.getToNode()) || this->isOffline(channel.getFromNode())){
            out << "  OFFLINE!!" << endl;
        }
    }
    out << flush;
}

void Network::printKeyspaces() {
    printKeyspaces(cout, ' ');
}
void Network::printKeyspaces(ostream &out, char spacer) {
    out << "KEYSPACES\n";

    int counter = 0;
    for (auto const& x : nodes) {
        for (const Keyspace &keyspace : x.second->getKeySpace()) {
            out << "Node" << spacer << "UUID" << spacer << "Keyspace" << "Start" << spacer << "End" << spacer
                << "Suffix Bits\n"
                << counter << spacer << x.second->getUUID() << spacer
                << keyspace.getStart() << spacer << keyspace.getEnd() << spacer
                << keyspace.getSuffix()
                << endl;
        }
        counter++;
    }
    out << flush;
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
cout << "total keyspace is " << total * 100 << "% of the keyspace" << endl;
    return total; 
}
