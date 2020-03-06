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
    double createdDay;
    double createdWeek;
    double longTermAllocation;
    double shortTermAllocation;
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

Message newBaseMessage(HexDigest &sendingUUID, HexDigest &destUUID, uint32_t lastReceived,
                       Message::ChannelState channelState, uint64_t msgID, long unixTimestamp = -1);
void addCollectionInfoRecord(InformationalMessageContents::CollectionInformationRecord *collection,
                             double createdDay, double createdWeek, double longAlloc, double shortAlloc);
void toInformationalMessage(Message &msg, std::initializer_list<CollectionInfoRecord> records);
void toKeyspaceMessage(Message &msg, std::initializer_list<KeyspaceExchangeRecord> records);

#endif //ADAK_KEYING_MESSAGE_HPP
