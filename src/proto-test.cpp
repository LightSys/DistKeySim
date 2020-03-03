#include <chrono>
#include <fstream>
#include <iostream>
#include "message.pb.h"

using namespace std;

int main(int argc, char **argv) {
    // Verify that protobuf version imported matches locally installed version
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    std::cout << "Creating Informational Message..." << std::endl;

    // Create contents of message
    InformationalMessageContents infoMsg;
    InformationalMessageContents_CollectionInformationRecord *infoRecord = infoMsg.add_records();
    auto *creationRateData = new InformationalMessageContents_CollectionInformationRecord_CreationRateData();
    creationRateData->set_createdpreviousday(0.0);
    creationRateData->set_createdpreviousweek(0.0);
    creationRateData->set_longallocationratio(1.0);
    creationRateData->set_shortallocationratio(1.0);
    infoRecord->set_collectionname("test");
    infoRecord->set_allocated_creationratedata(creationRateData);

    // Generate message from contents
    MessageInfo msg;
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

    // Create output stream for writing buffer contents to disk
    fstream output(argv[1], ios::out | ios::trunc | ios::binary);

    // Output buffer to disk
    if (!msg.SerializeToOstream(&output)) {
        cerr << "Failed to output to disk" << endl;
        return -1;
    }

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
