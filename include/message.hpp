// Provides helper functionality for converting to/from protobuf
#ifndef ADAK_KEYING_MESSAGE_HPP
#define ADAK_KEYING_MESSAGE_HPP

#include <chrono>
#include <string>
#include "UUID.h"
#include "message.pb.h"

/**
 * Represents Collection Information Record used in Informational Message
 */
struct CollectionInfoRecord {
    std::string name;
    float createdDay;
    float createdWeek;
    float longTermAllocation;
    float shortTermAllocation;
};

/**
 * Represents Keyspace Exchange Message used in Message
 */
struct KeyspaceExchangeRecord {
    std::string name;
    uint32_t startID;
    uint32_t endID;
    uint32_t suffixBits;
    
};

Message newBaseMessage(const HexDigest &sendingUUID, const HexDigest &destUUID, uint32_t lastReceived,
                       Message::ChannelState channelState, uint64_t msgID, long unixTimestamp = -1);
void addCollectionInfoRecord(InformationalMessageContents::CollectionInformationRecord *collection,
                             float createdDay, float createdWeek, float longAlloc, float shortAlloc);
void toInformationalMessage(Message &msg, std::initializer_list<CollectionInfoRecord> records);
void toKeyspaceMessage(Message &msg, std::initializer_list<KeyspaceExchangeRecord> records);

#endif //ADAK_KEYING_MESSAGE_HPP
