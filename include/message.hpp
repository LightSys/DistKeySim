// Provides helper functionality for converting to/from protobuf
#ifndef ADAK_KEYING_MESSAGE_HPP
#define ADAK_KEYING_MESSAGE_HPP

#include <string>
#include "message.pb.h"

InformationalMessageContents generateInformational() {}
KeyspaceMessageContents generateKeyspace() {}

/**
 * Adds a collection information record to an informational message contents instance
 * @param msg Informational message contents instance to add record to
 * @param createdDay
 * @param createdWeek
 * @param longAlloc
 * @param shortAlloc
 */
void addCollectionInformationRecord(InformationalMessageContents &contents, std::string &&collectionName, float createdDay, float createdWeek, float longAlloc, float shortAlloc) {
    auto *creationRateData = new InformationalMessageContents_CollectionInformationRecord_CreationRateData();
    creationRateData->set_createdpreviousday(createdDay);
    creationRateData->set_createdpreviousweek(createdWeek);
    creationRateData->set_longallocationratio(longAlloc);
    creationRateData->set_shortallocationratio(shortAlloc);
    infoRecord->set_collectionname(collectionName);
    infoRecord->set_allocated_creationratedata(creationRateData);
}

Message generateMessage() {
    InformationalMessageContents infoMsg;
    InformationalMessageContents_CollectionInformationRecord *infoRecord = infoMsg.add_records();
    addCollectionInformationRecord(infoRecord);

    // Generate message from contents
    Message msg;
    msg.set_sourcenodeid("14a89526-82e3-46f4-9fe7-f2d90ae7a84f");
    msg.set_destnodeid("14a89526-82e3-46f4-9fe7-f2d90ae7a84f");
    msg.set_lastreceivedmsg(0);
    msg.set_channelstate(MessageInfo::ChannelState::MessageInfo_ChannelState_INITIAL_STARTUP);

    // Current time
    google::protobuf::Timestamp time;
    long unixTS = chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()
    ).count();
    time.set_seconds(unixTS);
    time.set_nanos(0);
    msg.set_allocated_timestamp(&time);

    // Messages start at ID 1
    msg.set_messageid(1);
    msg.set_messagetype(MessageInfo::MessageType::MessageInfo_MessageType_INFORMATION);

    return msg;
}

#endif //ADAK_KEYING_MESSAGE_HPP
