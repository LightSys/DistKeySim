
#include <algorithm>
#include "Node.h"
#include <chrono> 

using namespace std;

Node::Node(double lambda1, double lambda2, double lambda3, int latency, double netScl) 
	: uuid(new_uuid()), lastDay(NodeData()), lambda1(lambda1), lambda2(lambda2), lambda3(lambda3) {
    generateObjectCreationRateDistribution();
    changeConsumptionRate();
    currentTick = 0;
    networkLatency = latency;
    networkScale = netScl;
    createdDay = 0;
    createdWeek = 0;
    keysShared = 0;
    d1 = new geometric_distribution<>(1/this->lambda1);
    d2 = new geometric_distribution<>(1/this->lambda2);

}

Node::Node(const Keyspace &keySpace, double lambda1, double lambda2, double lambda3, int latency, double netScl) 
	: uuid(new_uuid()), lastDay(NodeData()), lambda1(lambda1), lambda2(lambda2), lambda3(lambda3) {
    keyspaces.push_back(keySpace);
    generateObjectCreationRateDistribution();
    changeConsumptionRate();
    currentTick = 0;
    networkLatency = latency; 
    networkScale = netScl;
    createdDay = 0;
    createdWeek = 0;
    keysShared = 0; 
    d1 = new geometric_distribution<>(1/this->lambda1);
    d2 = new geometric_distribution<>(1/this->lambda2);
}

Node::Node(){
   //nothing... 
}


//find the toatl % of keyspace held 
double Node::getKeyspacePercent(){
    double total = 0;
    int size = keyspaces.size();
    for(int i = 0 ; i < size ; i ++){
	 if(keyspaces[i].getEnd() != UINT_MAX ) continue;  
         total += keyspaces[i].getPercent();

    }
    return total; 
} 

//calulate the number of keys held in full blocks
unsigned long long int Node::getTotalKeyspaceBlockSize(){
      double total = 0;
      int size = keyspaces.size();
      for(int i = 0 ; i < size ; i ++){
            total += keyspaces[i].getSize();
      }
      return total;
}


void Node::consumeObjects(){
    amountOfOneKeyUsed += objectConsuptionRatePerSecond;
//cout << "#############" <<  this->uuid << "consumption per second: " << objectConsuptionRatePerSecond << endl;
 
//make sure can consume keys.
    if(keyspaces.size() == 0) return;
    if(amountOfOneKeyUsed >= 1.0){
	cout << "consuming a key!! " << endl; 
        this->getNextKey();
        amountOfOneKeyUsed--;
         	
       	//update createdWeek to reflect the current history 
        
        createdDay = 0;
        //note 86400 is seconds in a day
	double max = 86400/unitsPerDay;

        if(max > history.size()){
           max = history.size(); 
	}
        
	//add up the number of logs to get a day
	//newest are at the rear of history
	for(int i = max - 1; i > 0 ; i--){
          createdDay +=  history[i].getKeysUsed();
        }

        createdWeek = 0;
    	int size = history.size();
        
	//add up teh full history, which loggs up to a week
        for(int i = 0 ; i < size ; i ++){
           createdWeek += history[i].getKeysUsed();
        }

	//add in the current progress this timestep (name lastDay is from older structure) 
        createdDay += lastDay.getKeysUsed();
        createdWeek += lastDay.getKeysUsed(); 
    }
    //set new somsumption rate: (should make it match  its lambda3 better
    changeConsumptionRate(); 
}

void Node::generateObjectCreationRateDistribution(){
    d3 = new geometric_distribution<>(1/this->lambda3);

    // These are required for the geometric distribution function to operate
    // correctly
    random_device rd;
    // Save the seed for debugging
    unsigned int seed = rd();
    gen = new mt19937(seed);
}


double Node::getTimeOnline(){
    return (1 + (*d2)(*gen));
}

double Node::getTimeOffline(){
    return (1 + (*d1)(*gen));
}

void Node::changeConsumptionRate(){
    objectConsuptionRatePerSecond = 1.0/(1 + (*d3)(*gen));
}


static Node rootNode(double lambda1, double lambda2, double lambda3, int latency, double networkScale) {
    return Node(Keyspace(0, UINT_MAX, 0), lambda1, lambda2, lambda3, latency, networkScale);
}

void Node::addPeer(const UUID &peerUUID) {
    if (peers.find(peerUUID) != peers.end()) {
        // Found a match, don't add duplicate connections
        return;
    }

    peers.insert({peerUUID, pair(nullptr, 1)});
}

void Node::addPeer(shared_ptr<Node> peer) {
    if (peers.find(peer->getUUID()) != peers.end()) {
        // Found a match, don't add duplicate connections
        return;
    }

    peers.insert({peer->getUUID(), pair(nullptr, 1)});
}

void Node::removePeer(shared_ptr<Node> peer) {
    auto foundPeer = peers.find(peer->getUUID());
    if (foundPeer != peers.end()) {
        // Found a match, don't add duplicate connections
        return;
    }

    // Found match, remove it
    peers.erase(foundPeer);
}

void Node::removePeer(const UUID &peerUUID) {
    // Search for local peer matching UUID
    auto foundPeer = peers.find(peerUUID);
    if (foundPeer != peers.end()) {
        // No peers that match that UUID exist
        return;
    }

    // Found match, remove it
    peers.erase(foundPeer);
}

shared_ptr<NodeData> Node::getNodeData() const {
    return make_shared<NodeData>(lastDay);
}

ADAK_Key_t Node::getNextKey() {
    lastDay.useKey();
    int index = minimumKeyspaceIndex();
    if (index == -1){
        ///(This was written from the first code-a-thon team to the second)
        ///Baylor This does mean there is an error in the code its more in how to distibute Keys correctly.
        ///Usually this happens in long term allocation statistics because it goes through till the end of
        ///the keyspace to see if th allocation is an issue. However if this happens. The problem is that the keys are
        ///getting distributed more than the current node has capcity for.

        string message = "ERROR from getNextKey in Node: No more keys to give";
        cout << message <<endl;
        Logger::log(message);
        return -1;
    } else {
	int key = keyspaces.at(index).getNextAvailableKey();
	if(!keyspaces.at(index).isKeyAvailable()){
           keyspaces.erase(keyspaces.begin() + index);
	}
        return key;
    }
}

int Node::minimumKeyspaceIndex(int newMin) {
    unsigned long min = ULONG_MAX;
    int index = -1;
    double start;
    double end;
    double suffix;
    for (int i = 0; i < keyspaces.size(); i++){
	start = keyspaces.at(i).getStart();
	end = keyspaces.at(i).getEnd();
	suffix = keyspaces.at(i).getEnd();
        if(start < min && keyspaces.at(i).isKeyAvailable() && start > newMin){
	// if (keyspaces.at(i).getStart() < min && keyspaces.at(i).isKeyAvailable() &&
	//	      (double) 	keyspaces.at(i).getStart() > (double) newMin) {
            min = keyspaces.at(i).getStart();
            index = i;
        }else if(! keyspaces.at(i).isKeyAvailable()){
            cout << uuid << " has invalid keyspace " << keyspaces[i].getStart() << "/" << keyspaces[i].getSuffix() << ", end " 
	<< keyspaces[i].getEnd() << endl; 
	}else if(!(start > newMin) && newMin == -1){
            cout << uuid << " has keyspace " << start << "/" << suffix << ", end " << end << ". Start is <= "<< newMin << endl; 
	}
    }
    return index;
}

bool Node::isKeyAvailable(){
    bool isAKey = false;
    for(const Keyspace& keys: this->keyspaces ){
        if(keys.isKeyAvailable()){
            isAKey = true;
            break;
        }
    }
    return isAKey;
}

/**
 * This is where the Baylor team will add more statistics to handle the messages that each node receives
 * @param message
 */

void Node::tick(){
    if(lastDay.getTimeUnitsPast() >= unitsPerDay){
      history.push_back(lastDay);
      //604800 is the seconds in a week
  
      if (history.size() > (604800.0/unitsPerDay)) {
            // Remove the first value from the vector, shifting the time, so we only store 1 week
            history.erase(history.begin());
        }
    
    }
    currentTick ++;
    //also log heartbeat
    logInfoForHeartbeat(); 
    
    lastDay.incTimeUnitPast();
}

void Node::heartbeat() {   

    //update the allocation 
    lastDay.updateLongTermAllocationRatio(keyspaces);
    lastDay.updateShortTermAllocationRatio(keyspaces);
  
    // No peers connected, so send
    if (peers.empty()) {
        sendQueue.push_back(getHeartbeatMessage(BROADCAST_UUID));
        messageID++;
        return;
    }

    for (const auto &[uuid, _] : peers) {
        // Create heartbeat message for each peer
        sendQueue.push_back(getHeartbeatMessage(uuid)); 
    }

}

bool Node::receiveMessage(const Message &message) {
    //the other message is const, so just edit a copy
    Message msg = message; 

//cout << "dealing with message " << msg.messageid() <<", confirmation " << msg.lastreceivedmsg() << endl; 

    if (msg.sourcenodeid() == uuid) {
        // Destination node is this node, don't receive it
        cout << "rejected a message, was sent to self " << endl;
        return false;
    }  
    
   
    // Handle last received message ID incrementing
    auto peer = peers.find(msg.sourcenodeid());
    if (peer == peers.end()) {
	//make a pointer out of msg
	Message *temp = new Message;
	*temp = msg;
        // First message received from this peer, add message ID and update find result
        peers.insert({msg.sourcenodeid(),pair(temp, 1)}); //now just copies message
        peer = peers.find(msg.sourcenodeid());
    } else if(peer->second.first == nullptr){
        //has no previouse message, set one 
	Message *temp = new Message;
        *temp = msg;
        // Known peer, update msg ID. Delete old message first to ovoid leaks 
        peers[msg.sourcenodeid()].first = temp; 
    } else {
	//make a pointer out of message
	//if it is a keyspace message, place id on the the old heartbeat; need the heartbeat data
	if(msg.messagetype() == Message::MessageType::Message_MessageType_KEYSPACE){
           peers[msg.sourcenodeid()].first->set_messageid(msg.messageid());
	   peers[msg.sourcenodeid()].first->set_lastreceivedmsg(msg.lastreceivedmsg());
	} else{
           //perforn the normal update
	   uint64_t lastID = peers[msg.sourcenodeid()].first->messageid();
	   if(msg.messageid() != lastID +1){
              //a message was missed. Take new info, but carry over confirmation and ID
//cout << uuid << "'s received message id " << msg.messageid() << " confirm " << msg.lastreceivedmsg();
//cout << " (from " << msg.sourcenodeid() << ")";
              uint64_t temp = (peers[msg.sourcenodeid()].first->messageid());
	      msg.set_messageid(temp);
	      temp = (peers[msg.sourcenodeid()].first->lastreceivedmsg());
	      msg.set_lastreceivedmsg(temp);
//cout << " is now message id " << msg.messageid() << " confirm " << msg.lastreceivedmsg() << endl;
	   }
  	   Message *temp = new Message;
           *temp = msg;
           // Known peer, update msg ID. Delete old message first to ovoid leaks 
           delete peers[msg.sourcenodeid()].first; 
           peers[msg.sourcenodeid()].first = temp; 

	}
    }

//cout << uuid << " updated last message to " << msg.messageid() << " from " << msg.sourcenodeid() << endl; 

    //Find out if the conifrmation matches
   // Message* lastMessage = NULL;
   // map<uint64_t, Message*>::iterator iter;
int loops = 0;
    //Remove all of the messages msg confirms. If a newer message is old enough and was not confimed, resend it
    
    //cout << uuid << " confirmed message " << tempID << " from " <<  confirmMsg[tempID]->sourcenodeid() << endl;
      
     //remove all messages with ID's older than confirmed from the same peer
     for(auto iter = confirmMsg.begin() ; iter != confirmMsg.end() ; iter++){ 
        //if it was sent before the confirmed one and came from the peers that just confirmed it, delente it
        if(iter->second == nullptr){
            //this is a problem, and should not be possible
	    cout << "ERROR: confirm message somehow got a null" << endl; 
	    cout << "message was suposed to be id " << iter->first << endl;
	    cout << "can find with id: " << (confirmMsg.find(iter->first) != confirmMsg.end()) << endl; 
           // break;  
	   throw 1; //just give up... 
	}else  if(confirmMsg.find(iter->first) == confirmMsg.end()){
           cout << "The value " << iter->first <<  " is not in the map... " << endl;
           throw 1; //no point in going on...
	}

	if(iter->second->destnodeid() == msg.sourcenodeid()){
           //if it reaches here, the message waiting to be confirmed was from the sender
           if(msg.lastreceivedmsg() >= iter->second->messageid()){
	      //confirm the messae
	      uint64_t tempID = iter->second->messageid();
//cout << "confirming message " << tempID << endl; 

              delete confirmMsg[tempID];

              //avance iter and then delete old location
	      auto tempIt = iter;
	      iter++;
	      confirmMsg.erase(tempIt);
              
	    } else {      
              //the message was meant for sender, but was not confirmed. See if it is old enough to resend
	       if(currentTick - timeStamps[iter->first] >=  2*networkLatency){
                  cout << "resending message " << iter->first << endl;
                  sendQueue.push_back(*(iter->second));
		  //update time stamp to prevent spamming resends 
		  timeStamps[iter->first] = currentTick;  
	       }
	    }  
         }
	loops ++;
     }

    //if there is still an uncorfimed message from the peer that is old enough, resend it 


    if(msg.messagetype() == Message_MessageType_KEYSPACE){
//cout << "was a keyspace" << endl; 
       // Receiving keyspace from a peer, generate new one for local store
       for (auto &&j =  0; j < msg.keyspace().keyspaces_size(); j++) {
           KeyspaceMessageContents::Keyspace peerSpace = msg.keyspace().keyspaces(j);
           Keyspace newSpace = Keyspace{peerSpace.startid(), peerSpace.endid(), peerSpace.suffixbits()};
// cout << "collected keysapce" << endl;
           //make sure keyspace is not a duplicate
           int size = keyspaces.size();
	   bool isNew = true;
	   for(int i = 0 ; i < size ; i ++){
              if(keyspaces[i].getSuffix() == newSpace.getSuffix() && keyspaces[i].getStart() == 
			   newSpace.getStart() && keyspaces[i].getEnd() == newSpace.getEnd()){
	         isNew = false;
                 cout << uuid << " found duplicate keyspace " << newSpace.getStart() << "/" << newSpace.getSuffix() << 
	              ", " << newSpace.getEnd() << endl; 
	         break;  
	      } 
	   }
	   if(isNew){
	      keyspaces.emplace_back(newSpace);
              totalLocalKeyspaceSize+=newSpace.getSize(); //updating total after recievin
	
//cout << uuid << " recived keyspace " << keyspaces.back().getStart() << "/" << keyspaces.back().getSuffix()
//       	<< " from " << msg.sourcenodeid() << endl; 
	   } else {
              cout << "duplicate keyspace recived by " <<  uuid << " from " << msg.sourcenodeid()
               << " in message " << msg.messageid() << endl;
	   }

	}
    }

    return false;
}

deque<Message> Node::getMessages() {
    deque<Message> toSend = move(sendQueue);
    sendQueue.empty();
    return toSend;
}

deque<Message> Node::checkMessages() {
    return sendQueue;
}

Message Node::getHeartbeatMessage(const UUID &peerID) {
    Message msg;
    if (peerID == BROADCAST_UUID) {
        // Broadcasting
        msg = newBaseMessage(
	    -1, //just always send a negative 
            uuid,
            peerID,
            0,
            Message_ChannelState_INITIAL_STARTUP
        );

    } else {
        Message* lastReceived = peers.at(peerID).first;
        msg = newBaseMessage(
            getNextMsgId(peerID),
	    uuid,
            peerID,
            lastReceived == nullptr ? 0 : lastReceived->messageid(),  
	    Message_ChannelState_NORMAL_COMMUNICATION    
        );   
    }
    
    // HACK
    /// NOTE, this is basically fixed in the develop branch, it is only here right now to tell other nodes,
    /// "If I don't have a keyspace, then give me a keyspace", this is because 1.0 is above the treshold ALLOCATION_BEFORE_GIVING_KEYSPACE
    if (keyspaces.empty()) {
        toInformationalMessage(
           msg,
           {
               CollectionInfoRecord{"test", calcLongAggregate(peerID), calcShortAggregate(peerID), 1, 1},
           }
       );

    }else{

    toInformationalMessage(
           msg, 
           {
	       //use the update keyspace functions so that what is being recived is the most up to date possible
               CollectionInfoRecord{"test", calcLongAggregate(peerID), 
	       calcShortAggregate(peerID), lastDay.getLongTermAllocationRatio(), 
	       lastDay.getShortTermAllocationRatio()},
           }
       );
    }

    //log time of message creation
    timeStamps[msg.messageid()] = currentTick; 

    //uint64_t temp = (uint64_t) msg.messageid();
    Message *temp = new Message;
    *temp = msg;
    confirmMsg[temp->messageid()] = temp;
 
//cout << "Node " << uuid << " made a heartbeat message " << msg.messageid() << " for " << peerID << endl; 
    return msg;
}

void Node::logInfoForHeartbeat(){
    vector<string> dataLine; //line in the csv
    dataLine.push_back(uuid);
    dataLine.push_back(to_string((Logger::getTimeslot(false))));
    ADAK_Key_t totalSize =0;
    long double percent = getKeyspacePercent();
    /*
    for(Keyspace k: keyspaces){
       // totalSize += k.getSize();
       tot++;
    }*/
    dataLine.push_back((to_string(percent)));
    
    dataLine.push_back(to_string(keysShared));
    //reset keys shared 
    keysShared = 0; 

    dataLine.push_back(to_string(objectConsuptionRatePerSecond));
    //dataLine.push_back(to_string(Logger::getShared(false,0)));
    //dataLine.push_back(to_string(Logger::getConsumption(false,0)));
    if(getTotalLocalKeyspaceSize() >0){
        ADAK_Key_t localSize = getTotalLocalKeyspaceSize();
        unsigned long long int  total  = getTotalKeyspaceBlockSize();
        dataLine.push_back(to_string(total));
    }else{
        dataLine.push_back("0.0");
    }
//cout << "Test\n"; 
    Logger::logStats(dataLine);
    //maybe put if ran out of space here, and uuid
}

//have the function return the index of the newly made half
int Node::splitKeyspace(int keyspaceIndex){
    if(keyspaceIndex > keyspaces.size() - 1){
       //invalid index, cannot split
       cout << "Invalid index for keyspace split" << endl; 
       return -1;
    } 

    //get the value to split and get needed information
    Keyspace toSplit = keyspaces.at(keyspaceIndex);
    uint64_t suffix = toSplit.getSuffix() + 1;           //increment the sufix
    uint64_t start1 = toSplit.getStart();                //the first starts on the same value
    uint64_t start2 = start1 + pow(2, suffix -1);        //the next starts on the first start + 2^oldSuffix
    uint64_t end = toSplit.getEnd(); 
    //the end stays the same

    //Warn if splitting a sub block
    if(end != UINT_MAX){
      cout << "warning: node " << uuid << " is splitting sub-block " << start1 << "/" << suffix << ", " << end << endl; 
    }

    Keyspace key1(start1, end, suffix);        
    Keyspace key2(start2, end, suffix); 
    
    //replace the old keyspace and add the new one
    keyspaces[keyspaceIndex] = key1;
    keyspaces.push_back(key2);
    
    //check validity of pieces make
    if(!key1.isKeyAvailable()){
       cout << "Made first half: " << key1.getStart() << "/" << key1.getSuffix() <<  ", " << key1.getEnd() << endl;
       throw 1;
    } else if(!key2.isKeyAvailable()){
       cout << "Made second half: " << key2.getStart() << "/" << key2.getSuffix() <<  ", " << key2.getEnd() << endl;
       throw 2;
    }

    return keyspaces.size() - 1;                     //the index of the new keyspace
}

void Node::sendCustKeyspace(UUID id, int keyInd){
    //prep the message
    Message keyMsg = newBaseMessage(getNextMsgId(id), uuid, id, peers[id].first->messageid(), Message_ChannelState_NORMAL_COMMUNICATION);
   
    //get the values of the keyspace to be sent
    uint64_t start = keyspaces[keyInd].getStart();
    uint64_t suffix = keyspaces[keyInd].getSuffix();
    uint64_t end = keyspaces[keyInd].getEnd();
    //remove keyspace form this node's array
    
    keyspaces.erase(keyspaces.begin() + keyInd);
   
    //log the amount about to share
    keysShared += 1.0/pow(2, suffix);

    //make the message a keyspace message
    KeyspaceExchangeRecord newRec = KeyspaceExchangeRecord{"share", start, end, (uint32_t) suffix};
    toKeyspaceMessage(keyMsg, {newRec});
     
    sendQueue.push_back(keyMsg);
 
//cout << "node " << uuid << " sent " << start << "/" << suffix << " to " << id << endl; 
    //add to confirmation list
    Message *temp = new Message;
    *temp = keyMsg;
    confirmMsg[temp->messageid()] = temp;
    timeStamps[keyMsg.messageid()] = currentTick;

}

//check if can send keyspace to a given node
bool Node::canSendKeyspace(UUID id){
    bool canSend = true;
    map<uint64_t, Message*>::iterator i;
    
    //search through confirmation and make sure that no keyspace messages are pending confirmation
    //from the node in question 
    for(i = confirmMsg.begin(); i != confirmMsg.end() ; i++){
        if( i->second->messagetype() != Message::MessageType::Message_MessageType_INFORMATION
			&& i->second->destnodeid() == id){	   
           //was a keyspace, cannot send another to that UUID
	   canSend = false;
        }
    }
 
    return canSend; 
}

void Node::sendCustKeyspace(UUID id, vector<int> keyInds){
    //prep the message
    Message keyMsg = newBaseMessage(getNextMsgId(id), uuid, id, peers[id].first->messageid(), Message_ChannelState_NORMAL_COMMUNICATION);
    vector<KeyspaceExchangeRecord> records;
    for(int i = keyInds.size() - 1 ; i >= 0 ; i --){
       //get the values of the keyspace to be sent
       uint64_t start = keyspaces[keyInds[i]].getStart();
       uint64_t suffix = keyspaces[keyInds[i]].getSuffix();
       uint64_t end = keyspaces[keyInds[i]].getEnd();
       //remove keyspace form this node's array
       keyspaces.erase(keyspaces.begin() + keyInds[i]);
       
       //log amount about to share
       keysShared += 1.0/pow(2, suffix);

       //make the message a keyspace message
       records.push_back(KeyspaceExchangeRecord{"share", start, end, (uint32_t) suffix});
//cout << uuid << "sent keyspace batch to " << id << " with " << start << "/" << suffix << ", " << end << endl; 
    }

    toKeyspaceMessage(keyMsg, records);
    
    sendQueue.push_back(keyMsg);

    //add to confirmation list
    Message *temp = new Message;
    *temp = keyMsg;
    confirmMsg[temp->messageid()] = temp;
    
//cout << uuid << " made multiple keyspace message  " << temp->messageid()  << " for " << id << endl;
     timeStamps[keyMsg.messageid()] = currentTick; 

}

vector<int> Node::makeSubBlock(int range){
   //make a subBlock of of the needed sze 
   //take from the minum keyspace 
   vector<int> indecies;

   //use as many blocks as need to
   int used = 0 ;
   int oldMin = -1;
   while(used < range){
      //get the smallest keyspace
      int mindex = minimumKeyspaceIndex(oldMin);
      if(mindex == -1){
          //the node does not have enough keyspace to finish...
	  cout << "Node " << uuid << " does not have enough keyspace to make sub-block size " << range << endl;
	  cout << "Number of keyspaces: " << keyspaces.size() << ", and old min: " << oldMin << endl; 
	  throw "Not enough keyspace to make sub-block";
	  //TODO: make a node exception class and throw it here... 
      }

      Keyspace temp = keyspaces[mindex]; 
      oldMin = temp.getStart(); 
      //if it is too small use the whole block, otherwise just take what need
      if(temp.getSize() <= range){
          used += temp.getSize();
//cout <<  uuid << "ussing sub block " << temp.getStart() << "/" << temp.getSuffix() << ", " << temp.getEnd() << endl;
	  //add the new location
	  indecies.push_back(mindex);
       }else{
          //can finish it off, so do so
	  int remainder = range - used;
	  used = range;
	  unsigned long start = temp.getStart();
	  unsigned long suffix = temp.getSuffix();
	  unsigned long end = temp.getEnd();
	  unsigned long joint = start + remainder*pow(2, suffix);
          keyspaces.push_back(Keyspace(start, joint, suffix));
//cout <<  uuid << "made sub block " << start << "/" << suffix << ", " << start  + remainder*pow(2,suffix) << endl;
//Keyspace tempII = keyspaces[keyspaces.size() -1];
//cout << uuid << " just make keyspace " << tempII.getStart() << "/" << tempII.getSuffix() << ", " << tempII.getEnd()
//	<< ".   Validity: " << tempII.isKeyAvailable() << endl; 
          //replace keyspace at mindex with one that does not include the keys used by the sub-block
	  keyspaces[mindex] = Keyspace(joint, end, suffix);
          //for(int i = 0 ; i < remainder ; i ++){
          //   keyspaces[mindex].getNextAvailableKey(); 
          //}
	  indecies.push_back(keyspaces.size() -1); //the deised keyspace is at the end of the vector
	  if(keyspaces[mindex].getStart() != keyspaces.back().getEnd()){
             cout << "Sub-Block creation error: full starts " << keyspaces[mindex].getStart() << ", sub ends " 
		    << keyspaces.back().getEnd() << endl; 
	     throw 3; 
	  }
      }
   }
   return indecies; 
}

double Node::calcLongAggregate(UUID target){
   //sum all of the long term calculations, but not the the one for target
   double result = 0;
   int numCounted = 1; 
   for(auto & peer: peers){
       if(peer.first == target || peer.second.first == nullptr){continue;}
       result += peer.second.first->info().records(0).creationratedata().createdpreviousweek();
       numCounted++;
   }
   result *= networkScale; 
   result += createdWeek;
   result /= numCounted; //# of peers + 1 for self - 1 for target
cout << uuid << " calcing long agg/ Week: " << createdWeek << ", numCounted: " << numCounted << endl;;  
   return result; 
}

double Node::calcShortAggregate(UUID target){
   //sum all of the short term calculations, but not the the one for target
   double result = 0;
   int numCounted = 1;
   for(auto & peer: peers){
       if(peer.first == target || peer.second.first == nullptr){continue;}
       result += peer.second.first->info().records(0).creationratedata().createdpreviousday();
       numCounted++;
   }
   result *= networkScale; 
   result += createdDay;
   result /= numCounted; //# of peers + self - target 
//cout << uuid << " calultaed aggregate short term " << result << endl; 
   return result; 
}

uint64_t Node::getNextMsgId(UUID peerID){
   int msgID = 0;
   if(peers.find(peerID) != peers.end()){
      msgID = peers[peerID].second++; //assign the ID and then incriment next
   }else {
      cout << uuid << " has no peer " << peerID << " so returning 0 for msg ID" << endl; 
   }
   
   return msgID; 
}
