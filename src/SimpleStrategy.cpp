#include "SimpleStrategy.h"
#include "Logger.h"
#include "Node.h"

using namespace std;

void SimpleStrategy::logKeySharing(UUID uuid, double longAlloc, double prevWeek) {
    vector<string> dataLine; //line in the csv
    dataLine.push_back(uuid);
    dataLine.push_back("N/A");
    dataLine.push_back(to_string(NAN));
    dataLine.push_back("N/A");
    dataLine.push_back(to_string(longAlloc));
    dataLine.push_back(to_string(areCloseEnough(1, longAlloc)));
    dataLine.push_back(to_string(NAN));
    dataLine.push_back(to_string(prevWeek));
    dataLine.push_back(to_string(NAN));
    dataLine.push_back(to_string(NAN));
    dataLine.push_back(to_string(NAN));
    Logger::logKeySharing(dataLine);
}

void SimpleStrategy::nodeTick(shared_ptr<Node> &node) {
    // TODO: implement heartbeats here
}

void SimpleStrategy::adak(Node &node, int keysToShift) {
    std::map<UUID, pair<Message *, uint64_t>> nodePeers = node.getPeers();
    std::map<UUID, pair<Message *, uint64_t>>::iterator i;
    long double avgProv = 0;
    long double avgKey = 0;
    int peersChecked = 0;
    // std::vector<std::pair<UUID, double>> defs;
    // double totalDef = 0;  // the deficit below the average

    vector<Keyspace> nodeKeyspaces = node.getKeySpace();
    Logger::log(Formatter() << "SS::adak: node=" << node.getUUID()
        << " number of peers=" << nodePeers.size()
        << " number of keyspaces=" << nodeKeyspaces.size());
    
    if (nodeKeyspaces.size() == 0) {
        return;
    }

    for (auto keyspace : nodeKeyspaces) {
        Logger::log(Formatter() << "  keyspace=" << keyspace.getStart()
            << "-" << keyspace.getEnd()
            << "/" << keyspace.getSuffix()
            << " (" << keyspace.getPercent()*100.0 << "%)");
    }

    // get my allocation data
    std::shared_ptr<NodeData> nodeData = node.getNodeData();

    double myLongAlloc = nodeData->updateLongTermAllocationRatio(nodeKeyspaces);
    double myPrevWeek = node.getCreatedWeek();

    // Check all peers and share keyspace if needed
    for (i = nodePeers.begin(); i != nodePeers.end(); i++) {
        Logger::log(Formatter() << "  peer=" << i->first);

        // get data from the nodes
        if (i->second.first == nullptr) {
            Logger::log("  no peer data");
            continue;
        }

        if (node.getKeySpace().size() == 0) {
            Logger::log("  we have no keyspace to share");
            return;
        }

        // get peer's allocation
        double peerLongAlloc =
            i->second.first->info().records(0).creationratedata().longallocationratio();
        double peerPrevWeek =
            i->second.first->info().records(0).creationratedata().createdpreviousweek();
        Logger::log(Formatter() << "  peerLongAlloc=" << peerLongAlloc
            << " myLongAlloc=" << myLongAlloc
            << " peerPrevWeek=" << peerPrevWeek
            << " myPrevWeek=" << myPrevWeek);

        SimpleStrategy::logKeySharing(node.getUUID(), peerLongAlloc, peerPrevWeek);

        // make sure sharing keyspace is actually possible
        if (!node.canSendKeyspace(i->first)) {
            Logger::log("  we can't send keyspace");
            continue;  // had no keys or target already received some keyspace, cannot share
        }

        if (myPrevWeek > 2 * peerPrevWeek) {
            Logger::log("  we intend to share");

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
                Logger::log(Formatter() << "  " << node.getUUID()
                  << " has only sub-blocks, and cannot split with " << i->first);
                continue;
            }

            // split largest in half
            int toSend = node.splitKeyspace(largest);
            nodeKeyspaces = node.getKeySpace();
            Logger::log("  splitting largest in half");
            node.sendCustKeyspace(i->first, toSend);

            break;  // shared half of keyspace, do not give any more
        } else {
            Logger::log(Formatter() << "  will not share keyspace because peerLongAlloc(" << peerLongAlloc
                << ") is not greater than 2 * myLongAlloc (" << 2*myLongAlloc << ")");
        }
        // Block sharing
    }
}

// comvert a fractional number to binary
string SimpleStrategy::fractToBin(long double fract, int accuracy) {
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
