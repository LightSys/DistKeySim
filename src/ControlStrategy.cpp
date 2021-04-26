#include "Logger.h"
#include "ControlStrategy.h"
#include "Node.h"
using namespace std;

//default value for accuracy
double ControlStrategy::accuracy = 0.0;

ControlStrategy::ControlStrategy(ClockType type, clock_unit_t heartbeatPeriod){
    nodeClock = shared_ptr<SystemClock>(SystemClock::makeClock(type));
}


void ControlStrategy::nodeTick(shared_ptr<Node>& node){
    // TODO: implement heartbeats here
}

void ControlStrategy::adak(Node &node, int keysToShift){
   std::map<UUID, pair<Message*, uint64_t>> nodePeers = node.getPeers();
   std::map<UUID, pair<Message*, uint64_t>>::iterator i;
   long double avgProv = 0;
   long double avgKey = 0;
   int peersChecked = 0;
   std::vector<std::pair<UUID, double>> defs; 
   double totalDef = 0; //the deficit below the average

   //Check all peers and share keysapce if needed
   for(i = nodePeers.begin() ; i != nodePeers.end() ; i++){
      //get data from the nodes
      if(i->second.first == nullptr){
	 continue;
      }

      if(node.getKeySpace().size() == 0){  
	  return; 
      }
      
      //get stats 
      double shortAlloc = i->second.first->info().records(0).creationratedata().shortallocationratio();
      double longAlloc  = i->second.first->info().records(0).creationratedata().longallocationratio();
      double prevDay    = i->second.first->info().records(0).creationratedata().createdpreviousday();
      double prevWeek   = i->second.first->info().records(0).creationratedata().createdpreviousweek();
      
      //store provisioning rate for block sharing, and keysace size for sub-block sharing
      avgProv += longAlloc/prevWeek; //big is good, small is bad
      avgKey  += shortAlloc; 
      peersChecked ++;

      //make sure sharing keyspace is actaully possible
     if(!node.canSendKeyspace(i->first)){
         continue; //had no keys or target already recived some keysapce, cannot share 
     }

     //Logger::log(Formatter() << "Node " << node.getUUID() << " lookinng at " << i->first << ": ");
//Logger::log(Formatter() << "Short: " << shortAlloc << ", long: " << longAlloc << ", day: " << prevDay << ", weel: " << prevWeek);
      //see of have no keysapce (these would both be aprox 1 if this is true)
      if(1 - shortAlloc < 0.000001 && 1 - longAlloc  < 0.000001 ){ 
         //give half of keyspace
        vector<Keyspace> nodeKeyspaces = node.getKeySpace();

	 //Find the largest keyspace
	 int largest = -1;
	 uint64_t minSuffix = 33; //This is too small to be possible with a uint64_t keyspace
	 for(int i = 0 ; i < nodeKeyspaces.size() ; i ++){
            //make sure not tp send sub blocks
            if((nodeKeyspaces[i].getSuffix() < minSuffix)  && (nodeKeyspaces[i].getEnd() == UINT_MAX)){
	       largest = i;
	       minSuffix = nodeKeyspaces[i].getSuffix();
	    }
	 }

	 //there is a chance it only has subblocks 
	 if(largest == -1){
             Logger::log(Formatter() << "Node " << node.getUUID() << "has only sub-blocks, and cannot split with node " << 
		    i->first);
	    continue; 
	 }

	 //split largest in half
	 int toSend = node.splitKeyspace(largest);
	 nodeKeyspaces = node.getKeySpace(); 
         node.sendCustKeyspace(i->first, toSend);

         continue; //shared half of keysapce, do not give any more 
      }
      //Block sharing 
     
   }

   //calculate averages
   avgProv /= peersChecked;
   avgKey  /= peersChecked;

   //determine block sharing 
   std::shared_ptr<NodeData> nodeData = node.getNodeData();

   vector<Keyspace> nodeKeyspaces = node.getKeySpace();

   double lngTrmAllc = nodeData->updateLongTermAllocationRatio(nodeKeyspaces);
   double lngAgg = node.calcLongAggregate(BROADCAST_UUID);
   double provRatio = lngTrmAllc/lngAgg;  
Logger::log(Formatter() << "long agg: " << lngAgg); 
Logger::log(Formatter() << "Node " << node.getUUID() << " with percent of global " << node.getKeyspacePercent() << " and local avg prov "
	<< avgProv << " has prov " << provRatio);
//Logger::log(Formatter() << "^^^^long alloc " << nodeData->getLongTermAllocationRatio() << ", week: " << node.getCreatedWeek()); 
//Logger::log(Formatter()<< node.getUUID() << " has " << node.getKeySpace().size() << " chuncks ");
//for(int n = 0 ; n < node.getKeySpace().size() ; n ++){
//Logger::log(Formatter() << "....... " << node.getKeySpace()[n].getStart() << "/" << node.getKeySpace()[n].getSuffix());  
//}

   if(avgProv < provRatio){
      //find keysapce excess ... Provisioning = object creation/keyspace
      long double excessKeys =(provRatio - avgProv)/provRatio*(1.05)*node.getKeyspacePercent();
      //need to try and average out, not just toss off extra
      excessKeys *= .5; 

      //never give away all of thier keyspace
      if(excessKeys/node.getKeyspacePercent() > 0.95){ 
         excessKeys = 0.95*node.getKeyspacePercent();
	 Logger::log(Formatter() << node.getUUID() << " is about to give away most of its keyspace... "); 
      }
      
      
Logger::log(Formatter() << "Node " << node.getUUID() << "Has determined excessKeys " << excessKeys << ", fraction of local " 
	<< node.getKeyspacePercent());
      
      //calculate the total deficit size for peers 
      for(i = nodePeers.begin() ; i != nodePeers.end() ; i++){
	  if(i->second.first == nullptr) continue;
	  long double longAlloc = i->second.first->info().records(0).creationratedata().longallocationratio();
          long double prevWeek   =  i->second.first->info().records(0).creationratedata().createdpreviousweek();
          long double shortAlloc = i->second.first->info().records(0).creationratedata().shortallocationratio();

	  //log defs for later portion here 
          if(longAlloc/prevWeek < avgProv && (longAlloc/prevWeek)/provRatio < 0.75  && !(1 - shortAlloc < 0.000001 && 1 - longAlloc  < 0.000001 )){
//FIXME: What about if the number of peers is 100? 1000? Defin in terms of accuracy (currently is 10... 
             defs.push_back(std::pair<UUID, 
			    long double>(i->second.first->sourcenodeid(), (avgProv - longAlloc/prevWeek)));
         //add to total deficit 
            totalDef += avgProv - longAlloc/prevWeek;
	  }
      }

      //find out what percentage of the global keysapce will be given, and give accordingly
      for(int j = 0 ; j < defs.size() ; j ++){
	 //update each node with their relative need   
         long double longAlloc = nodePeers[defs[j].first].first->info().
		 records(0).creationratedata().longallocationratio();
         long double prevWeek   =  nodePeers[defs[j].first].first->info().records(0).
		 creationratedata().createdpreviousweek();
	  if(defs[j].second/totalDef < 0.05){
//FIXME: What about if the number of peers is 100? 1000? Defin in terms of accuracy (currently is 10...   
//Logger::log(Formatter() << "node " << defs[j].first << " was kicked for haaving too little def:  " << defs[j].second);
             defs.erase(defs.begin() + j);
	     j--; 
	     continue; 
	  }


         long double fractOfDef = defs[j].second/totalDef; 
	 long double fractOfGlobalGive = excessKeys*fractOfDef;

Logger::log(Formatter() << "Node " << node.getUUID() << "Decided node " << defs[j].first << "should get % of def" << fractOfDef
 	<< ", which is percent of global " << fractOfGlobalGive);
         
         if(fractOfGlobalGive > node.getKeyspacePercent()){ 
	    Logger::log(Formatter() << "Giving away too much keyspace; percent to goive is greater than what have");
	    throw 1; 
         }  
	 //convert fraction to a sum of keyspaces via binary expansion
         //string bin = fractToBin(fractOfGlobalGive, accuracy);
         long double tempFract = fractOfGlobalGive;
	 //for each 1, send the corisponding keyspace
         for(int k = 0 ; k < accuracy ; k ++){
             //see if binary expansion of fract is a 1 or 0 at k-th location
             tempFract *= 2;
             int temp = tempFract;
             if(temp == 0){
		continue;  //was a 0, continue 
	     }
	     tempFract--;

	     //search for a keyspace for all of the 1's
	     int suffix = k + 1; 
	     std::vector<int> indices;
	     bool sentKeyspace = false;
	     nodeKeyspaces = node.getKeySpace();
             int numFound = 0;
	     int totalNeed = pow(2, accuracy - suffix);
             //accuracy serves as the smallest suffix
//Logger::log(Formatter() << node.getUUID() << " looking to send suffix " << suffix);;
	     //see if a keyspace >= the desired can be found one can be found
	     for(int l = 0 ; l < nodeKeyspaces.size() ; l++){
		 Keyspace tempSpace = nodeKeyspaces[l];
		 //skip if it is a sub-block
		 if(tempSpace.getEnd() < UINT_MAX){
//Logger::log(Formatter() << "Skip block " << tempSpace.getStart() << "/" << tempSpace.getSuffix() << ", " << tempSpace.getEnd());	
		    continue;
		 }

		 if(tempSpace.getSuffix() <= suffix){
//Logger::log(Formatter() << "..... Scratch it all, found one larger: " << tempSpace.getStart() << "/" << tempSpace.getSuffix());
		     //split until the suffix is correct 
	             while(node.getKeySpace()[l].getSuffix() < suffix ){
                        l = node.splitKeyspace(l);
                     }
//Keyspace bob = node.getKeySpace()[l];		     
//Logger::log(Formatter() << ".... .... Sent keyspace " << bob.getStart() << "/" << bob.getSuffix() << ", " << bob.getEnd()); 
                     //clear the indicies vector and store this index inside, and then exit the loop
		     indices.clear();
		     indices.push_back(l);
		     nodeKeyspaces = node.getKeySpace(); //update the nodeKeyspaces for next time 
		     break; 
		  } else if(pow(2, accuracy - tempSpace.getSuffix()) > (totalNeed - numFound)){
	             //is more than need, just finish it off and send
                     //take deficit, and turn it into a check list
//Logger::log(Formatter() << ".... can finish it off; found keyspace " << tempSpace.getStart() << "/"  << tempSpace.getSuffix() );
	             int def = totalNeed - numFound; 
                     int index = l;

		     for(int i = tempSpace.getSuffix() + 1 ; i <= accuracy ; i++){
                         if(def >= pow(2, accuracy - i)){
 			    //split keyspace until we have the part needed
			    while(node.getKeySpace()[index].getSuffix() < i){
                               index = node.splitKeyspace(index);
			    } 
			    indices.push_back(index);
//Logger::log(Formatter() << ".... .... sending keyspace " << node.getKeySpace()[index].getStart() << "/" << 
//       	node.getKeySpace()[index].getSuffix());
                            def	-= pow(2, accuracy - i);		    
			 }

			 if(def == 0){
                            break; //done
			 }
		     }
		     //done looking for keyspace
                     break; 
		  }else {
                     //just tack it on and move forward
		     indices.push_back(l); 
		     numFound += pow(2, accuracy - tempSpace.getSuffix());
//Logger::log(Formatter() << ".... Just added keyspace " << node.getKeySpace()[l].getStart() << "/" << 
//	node.getKeySpace()[l].getSuffix()); 
		  }
		  if(numFound == totalNeed){
                     break; //the else could trigger this, so check it
		  }
	      }
	      node.sendCustKeyspace(defs[j].first, indices);
//Logger::log(Formatter() << ".... and just sent them off"); 
/*
	      //will only run if the above search failed. Know all are smaller
	      //construct keyspace need out of the apropriate number of smaller keyspaces 
	      int l = 1;
	      int numFound = 0;
	      while(!sentKeyspace){
                  numFound *= 2; //the previously found value would be twice as much now
                  //looking for 2^l keyspaces with suffix  desired suffix + l
	          for(int m = 0 ; m < node.getKeySpace().size() ; m ++){	
	             if(node.getKeySpace()[m].getSuffix() == suffix + l && numFound < pow(2, l)){
	                //skip jeyspace if it is a sublock 
			if(node.getKeySpace()[m].getEnd() < UINT_MAX) continue; 
		        indices.push_back(m); //store the index of keyspace
			numFound++;
		     }
                  }
		  if(numFound == pow(2, l)){ //if this fails, another loop will occur...
	             //send the keyspaces off and exit
//Logger::log(Formatter() << "Node " << node.getUUID() << "sent a keyspace with " << indices.size() << " fragments\n"; 
	             node.sendCustKeyspace(defs[j].first, indices);
	 	     sentKeyspace = true; 
	   	  }else if(numFound > pow(2, l)){
                      //should be impossible, this needs fixed
		      //FIXME: add a control stategy exception class
		      Logger::log(Formatter() << "too much keyspace; " << numFound << " out of " << pow(2, l));
		      throw 1; 
		  } 
         
	          l++; 
	      }
	      */
	 }
      }
   } else {
//Logger::log(Formatter() << node.getUUID() << " is the poorer of the two"); 
   }

   //sub block passing: 
   subBlocks(node, avgKey, keysToShift);
}


void ControlStrategy::subBlocks(Node &node, long double avgKeys, int keysToShift){ 
   //need keyspace to do anything
   if(node.getKeySpace().size() == 0) return;
   
   std::map<UUID, pair<Message*, uint64_t>>::iterator i;
   std::map<UUID, pair<Message*, uint64_t>> nodePeers = node.getPeers(); 
   std::shared_ptr<NodeData> nodeData = node.getNodeData();
   std::vector<std::pair<UUID, double>> defs; 
   double totalDef = 0; //the deficit below the average
   vector<Keyspace> nodeKeyspaces = node.getKeySpace();
   double provRatio = nodeData->updateShortTermAllocationRatio(nodeKeyspaces);
//Logger::log(Formatter() << "Node " << node.getUUID() << " has says local average keys is  " << avgKeys << ", and prov is " 
//   << provRatio << "\n^^^^short alloc " << nodeData->getShortTermAllocationRatio() << ", day: " 
//   << node.getCreatedDay()); 

   if(avgKeys > provRatio ){
//Logger::log(Formatter() << node.getUUID() << " wants to give keyspace...";
     //find the total deficit 
     for(i = nodePeers.begin(); i != nodePeers.end() ; i ++){
         if(i->second.first == nullptr){
	    continue;
         }
	 long double shortAlloc = i->second.first->info().records(0).creationratedata().shortallocationratio();
         long double longAlloc  = i->second.first->info().records(0).creationratedata().longallocationratio();
         //long double prevDay   = i->second.first->info().records(0).creationratedata().createdpreviousday();
	 if(shortAlloc > avgKeys && shortAlloc/provRatio > 1.05
			 &&  !(1 - shortAlloc < 0.000001 && 1 - longAlloc  < 0.000001 )){
            totalDef += shortAlloc - avgKeys;
	    if(!node.canSendKeyspace(i->first)) continue; //want factored in, but do not consider sending to. 
            defs.push_back(std::pair<UUID, long double>(i->second.first->sourcenodeid(), 0));
         } 
      }
//Logger::log(Formatter() << " defs size: " << defs.size() << "... ");
      for(int j = 0 ; j < defs.size() ; j ++){
          //update each node with their relative need   
         /*long double shortAlloc = nodePeers[defs[j].first].first->info().
		 records(0).creationratedata().shortallocationratio();
         long double prevDay   =  nodePeers[defs[j].first].first->info().records(0).
		 creationratedata().createdpreviousday();
	 if((avgKeys - shortAlloc/prevDay)/totalDef < 0.05){
            //anything less than 5% of the deficit is unimportant
            defs.erase(defs.begin() + j);
	    j--; //account for the slot just deleted
	    continue;
//Logger::log(Formatter() << " kicked a node for having too little"); 
	 }*/	 
	  //send keysToShift size sub block
	  //double localAlloc = nodeData->updateShortTermAllocationRatio(nodeKeyspaces);
	  double totalKeyspace = node.getTotalKeyspaceBlockSize();
	  if(totalKeyspace > keysToShift){
//Logger::log(Formatter() << "node " << node.getUUID() << " decided to grant a sublock to " << defs[j].first); 
              node.sendCustKeyspace(defs[j].first, node.makeSubBlock(keysToShift));
	  }
      } 
  } 
}

//comvert a fractional number to binary
string ControlStrategy::fractToBin(long double fract, int accuracy){
   std::string bin = "";
   int temp;
   for(int i = 0 ; i < accuracy ; i ++){
       fract *= 2;
       temp = fract;
       if(temp == 0){
	  //just adding a 0 to the string
	  bin += "0";
       }else{
	  //add a 1 to the string and subtract 1 from fract
	  bin += "1";
	  fract -= 1;
       }
   }
   return bin;
}
  
