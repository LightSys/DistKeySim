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
    ADAK_Key_t startID;
    ADAK_Key_t endID;
    uint32_t suffixBits;
    
};

/** Generate new base protobuf message instance. Preferrably use newInformationalMessage or newKeyspaceMessage
 *
 * @param sendingUUID UUID string for node sending message
 * @param destUUID UUID string of destination, set to 00000000-0000-0000-0000-000000000000 if peer node ID not known
 * @param lastReceived Highest received consecutive Message ID from destination node (acknowledgment)
 * @param channelState Channel state (initial startup, normal communication, or channel shutdown)
 * @param msgID Local message ID (64 bit integer, starting at 1)
 * @param unixTimestamp Timestamp for message. Leaving default -1 will set timestamp to current system time
 * @throw std::invalid_argument if msgID is 0
 * @return Message instance created. Need to add message contents (either Keyspace or Informational) to have valid message
 */
Message newBaseMessage(const HexDigest &sendingUUID, const HexDigest &destUUID, uint32_t lastReceived,
                       Message::ChannelState channelState, uint64_t msgID, long unixTimestamp = -1);

/**
 * Adds a collection information record to an informational message contents instance
 * @param collection Pointer to collection information instance
 * @param createdDay float Keys created per day over 24 hours: CS
 * @param createdWeek float indicating Keys created per day over 7 days: CL
 * @param longAlloc float indicating Long-term keyspace allocation ratio: AL
 * @param shortAlloc float indicating Short-term keyspace allocation ratio: AS
 */
void addCollectionInfoRecord(InformationalMessageContents::CollectionInformationRecord *collection,
                             float createdDay, float createdWeek, float longAlloc, float shortAlloc);

/** Add data to message instance to change it to valid informational message
 *
 * @param msg Unpopulated message instance to add informational records to
 * @param records Collection Information Records to be added to message
 */
void toInformationalMessage(Message &msg, std::initializer_list<CollectionInfoRecord> records);

/** Add data to message instance to change it to valid keyspace exchange message
 *
 * @param msg Unpopulated message instance to add keyspace exchange message to
 * @param records Keyspace exchange records to add to message
 */
void toKeyspaceMessage(Message &msg, std::initializer_list<KeyspaceExchangeRecord> records);

#endif //ADAK_KEYING_MESSAGE_HPP
