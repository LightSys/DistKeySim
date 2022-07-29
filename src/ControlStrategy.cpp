#include "ControlStrategy.h"
#include "areCloseEnough.h"
#include "Logger.h"
#include "Node.h"

using namespace std;

ControlStrategy::ControlStrategy(ClockType type, clock_unit_t heartbeatPeriod) {
    nodeClock = shared_ptr<SystemClock>(SystemClock::makeClock(type));
}

void ControlStrategy::logKeySharing(UUID uuid, double shortAlloc,
        double longAlloc, double prevDay, double prevWeek,
        double avgProv, double avgKey, double provShortRatio, double provLongRatio,
        int peersChecked) {

    vector<string> dataLine; //line in the csv
    dataLine.push_back(uuid);
    dataLine.push_back(to_string((Logger::getTimeslot(false))));
    dataLine.push_back(to_string(shortAlloc));
    dataLine.push_back(to_string(longAlloc));
    dataLine.push_back(to_string(prevDay));
    dataLine.push_back(to_string(prevWeek));
    dataLine.push_back(to_string(avgProv));
    dataLine.push_back(to_string(avgKey));
    dataLine.push_back(to_string(provShortRatio));
    dataLine.push_back(to_string(provLongRatio));
    dataLine.push_back(to_string(peersChecked));
    Logger::logKeySharing(dataLine);
}

void ControlStrategy::nodeTick(shared_ptr<Node> &node) {
    // TODO: implement heartbeats here
}

void ControlStrategy::adak(Node &node, int keysToShift) {
    std::map<UUID, pair<Message *, uint64_t>> nodePeers = node.getPeers();
    std::map<UUID, pair<Message *, uint64_t>>::iterator i;
    long double avgProv = 0;
    long double avgKey = 0;
    int peersChecked = 0;
    std::vector<std::pair<UUID, double>> defs;
    double totalDef = 0;  // the deficit below the average

    vector<Keyspace> nodeKeyspaces = node.getKeySpace();
    Logger::log(Formatter() << "CS::adak: node=" << node.getUUID()
        << " number of peers=" << nodePeers.size()
        << " number of keyspaces=" << nodeKeyspaces.size());
    
    if (nodeKeyspaces.size() == 0) {
        return;
    }

    for (auto keyspace : nodeKeyspaces) {
        Logger::log(Formatter() << "+-keyspace=" << keyspace.getStart()
            << "-" << keyspace.getEnd()
            << "/" << keyspace.getSuffix()
            << " (" << keyspace.getPercent()*100.0 << "%)");
    }

    // Check all peers and share keyspace if needed
    for (i = nodePeers.begin(); i != nodePeers.end(); i++) {
        Logger::log(Formatter() << "+-peer=" << i->first);

        // get data from the nodes
        if (i->second.first == nullptr) {
            Logger::log("+-No data; continue");
            continue;
        }

        if (node.getKeySpace().size() == 0) {
            Logger::log("+-My keyspace size is 0; continue");
            return;
        }

        // get stats
        double shortAlloc =
            i->second.first->info().records(0).creationratedata().shortallocationratio();
        double longAlloc =
            i->second.first->info().records(0).creationratedata().longallocationratio();
        double prevDay = i->second.first->info().records(0).creationratedata().createdpreviousday();
        double prevWeek =
            i->second.first->info().records(0).creationratedata().createdpreviousweek();
        Logger::log(Formatter() << "+-longAlloc=" << longAlloc << " shortAlloc=" << shortAlloc);

        // store provisioning rate for block sharing, and keysace size for sub-block sharing
        avgProv += longAlloc / prevWeek;  // big is good, small is bad
        avgKey += shortAlloc;
        Logger::log(Formatter() << "avgKey=" << avgKey << " += " << shortAlloc);
        peersChecked++;

        // make sure sharing keyspace is actually possible
        if (!node.canSendKeyspace(i->first)) {
            Logger::log("+-Can't send keyspace; continue");
            continue;  // had no keys or target already received some keyspace, cannot share
        }

        ControlStrategy::logKeySharing(i->first, shortAlloc,
            longAlloc, prevDay, prevWeek, avgProv, avgKey, NAN, NAN, peersChecked);

        Logger::log(Formatter() << "+-longAlloc=" << longAlloc << " shortAlloc=" << shortAlloc);
        
        // see if have no keyspace (these would both be approx 1 if this is true)
        if (ACE::areCloseEnough(1, shortAlloc) && ACE::areCloseEnough(1, longAlloc)) {
            Logger::log(Formatter() << "+-give half of keyspace to " << i->first
                << " because it appears to have none");

            // Find the largest keyspace
            int largest = -1;
            uint64_t minSuffix = 33;  // This is too small to be possible with a uint64_t keyspace
            for (int i = 0; i < nodeKeyspaces.size(); i++) {
                // make sure not to send sub blocks
                if ((nodeKeyspaces[i].getSuffix() < minSuffix) &&
                    (nodeKeyspaces[i].getEnd() == UINT_MAX)) {
                    largest = i;
                    minSuffix = nodeKeyspaces[i].getSuffix();
                }
            }

            // there is a chance it only has subblocks
            if (largest == -1) {
                Logger::log(Formatter() << "+-" << node.getUUID()
                  << " has only sub-blocks, and cannot split with " << i->first);
                continue;
            }

            // split largest in half
            int toSend = node.splitKeyspace(largest);
            nodeKeyspaces = node.getKeySpace();
            
            uint64_t start = nodeKeyspaces[toSend].getStart();
            uint64_t suffix = nodeKeyspaces[toSend].getSuffix();
            uint64_t end = nodeKeyspaces[toSend].getEnd();
            Logger::log(Formatter() << "+-splitting largest in half; sending "
                << start << "-" << end << "/" << suffix
                << " (" << nodeKeyspaces[toSend].getPercent()*100.0 << "%)"
                << " to " << i->first);

            node.sendCustKeyspace(i->first, toSend);

            return;  // shared half of keyspace, do not give any more
        } else {
            Logger::log(Formatter() << "+-will not share keyspace because shortAlloc and longAlloc are not 1");
        }
        // Block sharing
    }

    // calculate averages
    avgProv /= peersChecked;
    avgKey /= peersChecked;
    Logger::log(Formatter() << "avgKey=" << avgKey << " /= " << peersChecked);

    // determine block sharing
    std::shared_ptr<NodeData> nodeData = node.getNodeData();

    // Compute sum of keyspace percentages (AKA keyspace allocation ratio)
    double sumLongKeyspaceAlloc = nodeData->updateLongTermAllocationRatio(nodeKeyspaces);
    // Compute adjusted sum of long term allocation values
    double longAgg = node.calcLongAggregate(BROADCAST_UUID);
    // Compute Keyspace Demand-to-Allocation Ratio
    double provLongRatio =  longAgg / sumLongKeyspaceAlloc;

    Logger::log(Formatter() << "+-percent of global=" << node.getKeyspacePercent()*100.0
        << "% avgProv="  << avgProv
        << " sumLongKeyspaceAlloc="  << sumLongKeyspaceAlloc
        << " longAgg="  << longAgg
        << " provLongRatio="  << provLongRatio
        << " (avgProv < provLongRatio)="
        << ACE::toString(ACE::isLessThan(avgProv, provLongRatio)));

    // Compute sum of keyspace percentages (AKA keyspace allocation ratio)
    double sumShortKeyspaceAlloc = nodeData->updateShortTermAllocationRatio(nodeKeyspaces);
    // Compute adjusted sum of short term allocation values
    double shortAgg = node.calcShortAggregate(BROADCAST_UUID);
    // Compute Keyspace Demand-to-Allocation Ratio
    double provShortRatio =  shortAgg / sumShortKeyspaceAlloc;

    Logger::log(Formatter() << "+-percent of global=" << node.getKeyspacePercent()*100.0
        << "% avgProv="  << avgProv
        << " sumShortKeyspaceAlloc="  << sumShortKeyspaceAlloc
        << " shortAgg="  << shortAgg
        << " provShortRatio="  << provShortRatio
        << " (avgProv < provShortRatio)="
        << ACE::toString(ACE::isLessThan(avgProv, provShortRatio)));

    ControlStrategy::logKeySharing(node.getUUID(),
        nodeData->updateShortTermAllocationRatio(nodeKeyspaces),
        sumShortKeyspaceAlloc, NAN, NAN, avgProv, avgKey,
        provShortRatio, provLongRatio, peersChecked);

    if (ACE::isLessThan(avgProv, provShortRatio)) {
        // find keyspace excess ... Provisioning = object creation/keyspace
        long double excessKeys =
            (provShortRatio - avgProv) / provShortRatio * (1.05) * node.getKeyspacePercent();
        // need to try and average out, not just toss off extra
        excessKeys *= .5;

        // never give away all of thier keyspace
        if (excessKeys / node.getKeyspacePercent() > 0.95) {
            excessKeys = 0.95 * node.getKeyspacePercent();
            Logger::log(Formatter() << "+-" << node.getUUID()
               << " is about to give away most of its keyspace... ");
        }

        Logger::log(Formatter() << "+-Has determined excessKeys "
                                << excessKeys << ", fraction of local "
                                << node.getKeyspacePercent());

        // calculate the total deficit size for peers
        for (i = nodePeers.begin(); i != nodePeers.end(); i++) {
            if (i->second.first == nullptr) continue;
            double longAlloc =
                i->second.first->info().records(0).creationratedata().longallocationratio();
            double prevWeek =
                i->second.first->info().records(0).creationratedata().createdpreviousweek();
            double shortAlloc =
                i->second.first->info().records(0).creationratedata().shortallocationratio();
            double prevDay =
                i->second.first->info().records(0).creationratedata().createdpreviousday();
 
            ControlStrategy::logKeySharing(i->first, shortAlloc,
                longAlloc, prevDay, prevWeek, avgProv, avgKey,
                provShortRatio, provLongRatio, peersChecked);
            
            Logger::log(Formatter() << "+-longAlloc=" << longAlloc << " shortAlloc=" << shortAlloc
                << " prevWeek=" << prevWeek << " avgProv=" << avgProv << " provShortRatio=" << provShortRatio);

            // log defs for later portion here
            if (ACE::isLessThan(longAlloc / prevWeek, avgProv) &&
                ACE::isLessThan((longAlloc / prevWeek) / provShortRatio, 0.75) &&
                !(ACE::areCloseEnough(1, shortAlloc) && ACE::areCloseEnough(1, longAlloc))) {
                Logger::log("+-that mess was true");
                // FIXME: What about if the number of peers is 100? 1000? Defin in terms of accuracy
                // (currently is 10...
                defs.push_back(std::pair<UUID, long double>(i->second.first->sourcenodeid(),
                                                            (avgProv - longAlloc / prevWeek)));
                // add to total deficit
                Logger::log(Formatter() << "+-totalDef += avgProv - longAlloc / prevWeek");
                Logger::log(Formatter() << "+-totalDef += " << avgProv << " - " << longAlloc << " / " << prevWeek);
                totalDef += avgProv - longAlloc / prevWeek;
                Logger::log(Formatter() << "+-totalDef = " << totalDef);
            } else {
                Logger::log(Formatter() << "+-Did not compute defs for any peer");
            }
        }

        // find out what percentage of the global keysapce will be given, and give accordingly
        for (int j = 0; j < defs.size(); j++) {
            // update each node with their relative need
            // long double longAlloc = nodePeers[defs[j].first]
            //                             .first->info()
            //                             .records(0)
            //                             .creationratedata()
            //                             .longallocationratio();
            // long double prevWeek = nodePeers[defs[j].first]
            //                            .first->info()
            //                            .records(0)
            //                            .creationratedata()
            //                            .createdpreviousweek();
            if (defs[j].second / totalDef < 0.05) {
                // FIXME: What about if the number of peers is 100? 1000? Defin in terms of accuracy
                // (currently is 10... Logger::log(Formatter() << defs[j].first << " was
                // kicked for haaving too little def:  " << defs[j].second);
                defs.erase(defs.begin() + j);
                j--;
                continue;
            }

            Logger::log(Formatter() << "+-fractOfDef = defs[j].second / totalDef");
            Logger::log(Formatter() << "+-fractOfDef = " << defs[j].second << " / " << totalDef);
            long double fractOfDef = defs[j].second / totalDef;
            Logger::log(Formatter() << "+-fractOfDef = " << fractOfDef);
            long double fractOfGlobalGive = excessKeys * fractOfDef;

            Logger::log(Formatter() << "+-" << node.getUUID() << " Decided " << defs[j].first
                                    << " should get % of def " << fractOfDef
                                    << ", which is percent of global " << fractOfGlobalGive);

            if (fractOfGlobalGive > node.getKeyspacePercent()) {
                Logger::log(Formatter()
                    << "+-Giving away too much keyspace; percent to give is greater than what have");
                return;
            }
            // convert fraction to a sum of keyspaces via binary expansion
            // string bin = fractToBin(fractOfGlobalGive, accuracy);
            long double tempFract = fractOfGlobalGive;
            // for each 1, send the corisponding keyspace
            for (int k = 0; k < accuracy; k++) {
                // see if binary expansion of fract is a 1 or 0 at k-th location
                tempFract *= 2;
                int temp = tempFract;
                if (temp == 0) {
                    continue;  // was a 0, continue
                }
                tempFract--;

                // search for a keyspace for all of the 1's
                int suffix = k + 1;
                std::vector<int> indices;
                bool sentKeyspace = false;
                nodeKeyspaces = node.getKeySpace();
                int numFound = 0;
                int totalNeed = pow(2, accuracy - suffix);
                // accuracy serves as the smallest suffix
                Logger::log(Formatter() << node.getUUID() << " looking to send suffix " << suffix);
                // see if a keyspace >= the desired can be found one can be found
                for (int l = 0; l < nodeKeyspaces.size(); l++) {
                    Keyspace tempSpace = nodeKeyspaces[l];
                    // skip if it is a sub-block
                    if (tempSpace.getEnd() < UINT_MAX) {
                        Logger::log(Formatter() << "Skip block " << tempSpace.getStart() << "/"
                            << tempSpace.getSuffix() << ", " << tempSpace.getEnd());
                        continue;
                    }

                    if (tempSpace.getSuffix() <= suffix) {
                        Logger::log(Formatter() << "..... Scratch it all, found one larger: " <<
                        tempSpace.getStart() << "/" << tempSpace.getSuffix());
                        // split until the suffix is correct
                        while (node.getKeySpace()[l].getSuffix() < suffix) {
                            l = node.splitKeyspace(l);
                        }
                        Keyspace bob = node.getKeySpace()[l];
                        Logger::log(Formatter() << ".... .... Sent keyspace " << bob.getStart()
                            << "-" << bob.getEnd() << "/" << bob.getSuffix()
                            << " (" << bob.getPercent()*100.0 << "%)");
                        // clear the indicies vector and store this index inside, and then exit the loop
                        indices.clear();
                        indices.push_back(l);
                        nodeKeyspaces =
                            node.getKeySpace();  // update the nodeKeyspaces for next time
                        break;
                    } else if (pow(2, accuracy - tempSpace.getSuffix()) > (totalNeed - numFound)) {
                        // is more than need, just finish it off and send
                        // take deficit, and turn it into a check list
                        Logger::log(Formatter() << ".... can finish it off; found keyspace " <<
                            tempSpace.getStart() << "/" << tempSpace.getSuffix() );
                        int def = totalNeed - numFound;
                        int index = l;

                        for (int i = tempSpace.getSuffix() + 1; i <= accuracy; i++) {
                            if (def >= pow(2, accuracy - i)) {
                                // split keyspace until we have the part needed
                                while (node.getKeySpace()[index].getSuffix() < i) {
                                    index = node.splitKeyspace(index);
                                }
                                indices.push_back(index);
                                Logger::log(Formatter() << ".... .... sending keyspace " <<
                                node.getKeySpace()[index].getStart() << "/" <<
                                       	node.getKeySpace()[index].getSuffix());
                                def -= pow(2, accuracy - i);
                            }

                            if (def == 0) {
                                break;  // done
                            }
                        }
                        // done looking for keyspace
                        break;
                    } else {
                        // just tack it on and move forward
                        indices.push_back(l);
                        numFound += pow(2, accuracy - tempSpace.getSuffix());
                        Logger::log(Formatter() << ".... Just added keyspace " <<
                        node.getKeySpace()[l].getStart() << "/" <<
                        	node.getKeySpace()[l].getSuffix());
                    }
                    if (numFound == totalNeed) {
                        break;  // the else could trigger this, so check it
                    }
                }
                node.sendCustKeyspace(defs[j].first, indices);
                Logger::log(Formatter() << ".... and just sent them off");
                /*
                              //will only run if the above search failed. Know all are smaller
                              //construct keyspace need out of the apropriate number of smaller
                keyspaces int l = 1; int numFound = 0; while(!sentKeyspace){ numFound *= 2; //the
                previously found value would be twice as much now
                                  //looking for 2^l keyspaces with suffix  desired suffix + l
                                  for(int m = 0 ; m < node.getKeySpace().size() ; m ++){
                                     if(node.getKeySpace()[m].getSuffix() == suffix + l && numFound
                < pow(2, l)){
                                        //skip jeyspace if it is a sublock
                                        if(node.getKeySpace()[m].getEnd() < UINT_MAX) continue;
                                        indices.push_back(m); //store the index of keyspace
                                        numFound++;
                                     }
                                  }
                                  if(numFound == pow(2, l)){ //if this fails, another loop will
                occur...
                                     //send the keyspaces off and exit
                                     Logger::log(Formatter() << node.getUUID() << "sent a keyspace with " <<
                                        indices.size() << " fragments\n"; node.sendCustKeyspace(defs[j].first, indices);
                                     sentKeyspace = true;
                                  }else if(numFound > pow(2, l)){
                                      //should be impossible, this needs fixed
                                      //FIXME: add a control stategy exception class
                                      Logger::log(Formatter() << "too much keyspace; " << numFound
               << " out of " << pow(2, l)); throw 1;
                                  }

                                  l++;
                              }
                              */
            }
        }
    } else {
        Logger::log(Formatter() << "+-is the poorer of the two");
    }

    // sub block passing:
    subBlocks(node, avgKey, keysToShift);
}

void ControlStrategy::subBlocks(Node &node, long double avgKeys, int keysToShift) {
    Logger::log(Formatter() << "subBlocks(" << node.getUUID() << ", " << avgKeys << ", " << keysToShift << ")"
        << " keySpace.size=" << node.getKeySpace().size());
    // need keyspace to do anything
    if (node.getKeySpace().size() == 0) return;

    std::map<UUID, pair<Message *, uint64_t>>::iterator i;
    std::map<UUID, pair<Message *, uint64_t>> nodePeers = node.getPeers();
    std::shared_ptr<NodeData> nodeData = node.getNodeData();
    std::vector<std::pair<UUID, double>> defs;
    double totalDef = 0;  // the deficit below the average
    vector<Keyspace> nodeKeyspaces = node.getKeySpace();
    double provShortRatio = nodeData->updateShortTermAllocationRatio(nodeKeyspaces);

    Logger::log(Formatter() << "subBlocks: avgKeys(" << avgKeys << ") > provShortRatio(" << provShortRatio << ") = "
        << ACE::toString(ACE::isGreaterThan(avgKeys, provShortRatio)));

    if (ACE::isGreaterThan(avgKeys, provShortRatio)) {
        Logger::log(Formatter() << "+-wants to give keyspace...");
        // find the total deficit
        for (i = nodePeers.begin(); i != nodePeers.end(); i++) {
            if (i->second.first == nullptr) {
                continue;
            }
            double shortAlloc =
                i->second.first->info().records(0).creationratedata().shortallocationratio();
            double longAlloc =
                i->second.first->info().records(0).creationratedata().longallocationratio();
            Logger::log(Formatter() << "+-peer=" << i->first
                << " longAlloc=" << longAlloc << " shortAlloc=" << shortAlloc);

            // Commented out because not used
            // double prevDay   =
            // i->second.first->info().records(0).creationratedata().createdpreviousday();

            if (ACE::isGreaterThan(shortAlloc, avgKeys) && ACE::isGreaterThan(shortAlloc / provShortRatio, 1.05) &&
                !(ACE::areCloseEnough(1, shortAlloc) && ACE::areCloseEnough(1, longAlloc))) {
                totalDef += shortAlloc - avgKeys;
                if (!node.canSendKeyspace(i->first))
                    Logger::log(Formatter() << "+-" << node.getUUID() << " has no keyspace to send");
                    continue;  // want factored in, but do not consider sending to.
                defs.push_back(std::pair<UUID, long double>(i->second.first->sourcenodeid(), 0));
            } else {
                Logger::log(Formatter() << "+-Did not add "<< node.getUUID() << " to total deficits");
            }
        }
        if (defs.size() == 0) {
            Logger::log("+-But peer deficits is empty");
        } else {
            Logger::log(Formatter() << "+-defs size: " << defs.size() << "... ");
        }
        for (int j = 0; j < defs.size(); j++) {
            // update each node with their relative need
            /*double shortAlloc = nodePeers[defs[j].first].first->info().
                    records(0).creationratedata().shortallocationratio();
              double prevDay   =  nodePeers[defs[j].first].first->info().records(0).
                    creationratedata().createdpreviousday();
            if((avgKeys - shortAlloc/prevDay)/totalDef < 0.05){
               //anything less than 5% of the deficit is unimportant
               defs.erase(defs.begin() + j);
               j--; //account for the slot just deleted
               continue;
               Logger::log(Formatter() << " kicked a node for having too little");
            }*/
            // send keysToShift size sub block
            // double localAlloc = nodeData->updateShortTermAllocationRatio(nodeKeyspaces);
            double totalKeyspace = node.getTotalKeyspaceBlockSize();
            Logger::log(Formatter() << "+-totalKeyspace(" << totalKeyspace << ") "
                << "> keysToShift(" << keysToShift << ") = " << ACE::toString(totalKeyspace > keysToShift));
            if (totalKeyspace > keysToShift) {
                Logger::log(Formatter() << "+-decided to grant a sublock to " << defs[j].first);
                node.sendCustKeyspace(defs[j].first, node.makeSubBlock(keysToShift));
            }
        }
    }
}

// comvert a fractional number to binary
string ControlStrategy::fractToBin(long double fract, int accuracy) {
    std::string bin = "";
    int temp;
    for (int i = 0; i < accuracy; i++) {
        fract *= 2;
        temp = fract;
        if (temp == 0) {
            // just adding a 0 to the string
            bin += "0";
        } else {
            // add a 1 to the string and subtract 1 from fract
            bin += "1";
            fract -= 1;
        }
    }
    return bin;
}
