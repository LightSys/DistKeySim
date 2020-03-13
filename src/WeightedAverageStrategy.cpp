#include "WeightedAverageStrategy.h"
#include "SystemClock.h"

using namespace std;


WeightedAverageStrategy::WeightedAverageStrategy(
	ClockType type, clock_unit_t heartbeatPeriod,
	int keyspaceChunkSize, double diffusionRate, 
        double allocationThreshold
){
    this->keyspaceChunkSize = keyspaceChunkSize;
    this->diffusionRate = diffusionRate;
    this->allocationThreshold = allocationThreshold;
    this->nodeClock = SystemClock::makeClock(type);
    this->heartbeatPeriod = heartbeatPeriod;
    this->nodeClock->setTimer(W_AVG_HEARTBEAT_TIMER, heartbeatPeriod);
}

void WeightedAverageStrategy::nodeTick(Node* node){

    nodeClock.tick();

    if(nodeClock->checkTimer(W_AVG_HEARTBEAT_TIMER) == ENDED){

        //check new messages:
        queue<Messages> newMessages = node->getReceivedMessages();
        while(!newMessages.empty()){
            processMessage(newMessages.front());
            newMessages.pop();
        }

        node->heartbeat();
        nodeClock->setTimer(W_AVG_HEARTBEAT_TIMER, heartbeatPeriod);
    }
}

void processMessage(Node* node, const Message& msg){

    // If destination node is this node, don't receive it
    if (msg.sourcenodeid() == node->getUUID()) {
        return;
    }

    // Check time and update lastDay and rotate the history

    // Handle last received message ID incrementing
    auto peer = peers.find(msg.sourcenodeid());

    if (peer == peers.end()) {
        // First message received from this peer, add message ID and update find result
        peers.insert({msg.sourcenodeid(), make_shared<Message>(msg)});
        peer = peers.find(msg.sourcenodeid());
    } else {
        // Known peer, update msg ID
        peers.at(msg.sourcenodeid()) = make_shared<Message>(msg);
    }

    switch (msg.messagetype()) {
        case Message_MessageType_KEYSPACE:
            node->receiveKeyspace(msg);
            break;

        case Message_MessageType_INFORMATION:

            double shortAllocationRatio = 0.0;
            double longAllocationRatio = 0.0;

            for (int i = 0; i < msg.info().records_size(); i++) {
                shortAllocationRatio = msg.info().records(i).creationratedata().shortallocationratio();
                longAllocationRatio = msg.info().records(i).creationratedata().longallocationratio();
            }

            if (allocationRatio > ALLOCATION_BEFORE_GIVING_KEYSPACE) {
                if (peer == peers.end()) {
                    // No peer found matching UUID in message, ignore this message
                    break;
                }

                // Handle decision on giving of keyspace
                if (!keyspaces.empty()) {
                    Keyspace keyspaceToGive = keyspaces.at(minimumKeyspaceIndex());


                    // If there is at least one spot available
                    if ((keyspaceToGive.getStart() + pow(2, keyspaceToGive.getSuffix())) < keyspaceToGive.getEnd()) {
                        Message shareSpaceMsg = newBaseMessage(
                            uuid,
                            peer->first,
                            peer->second->lastreceivedmsg(),
                            Message_ChannelState_NORMAL_COMMUNICATION,
                            messageID++
                        );
                        shareKeyspace(shareSpaceMsg);

                        // Send message to peer
                        sendQueue.push_back(shareSpaceMsg);

                        return true;
                    }
                }
            }
        break;

        default:
            // Should never reach this point in normal operation
            throw invalid_argument("message invalid type");
    }

    return false;
}

