#include <iostream>
#include "message.hpp"
#include "UUID.h"

using namespace std;

int main(int argc, char **argv) {
    // Verify that protobuf version imported matches locally installed version
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Generate base unpopulated message timestamped at current time
    HexDigest uuidFrom = new_uuid();
    HexDigest uuidTo = new_uuid();
    Message msg = newBaseMessage(
        uuidFrom,
        uuidTo,
        0,
        Message::ChannelState::Message_ChannelState_INITIAL_STARTUP,
        1
    );
    
    // Add collection information record(s) to message
    toInformationalMessage(
        msg,
        {
            CollectionInfoRecord{"test", 0.0, 0.0, 1.0, 1.0},
            CollectionInfoRecord{"test2", 0.0, 0.0, 1.0, 1.0},
        }
    );

    // Create output stream for writing buffer contents to disk, defaulting to a.buf if not path provided
    std::string outPath = argc > 1 ? argv[1] : "a.buf";
    fstream output(outPath, ios::out | ios::trunc | ios::binary);

    // Output buffer to disk
    if (!msg.SerializeToOstream(&output)) {
        cerr << "Failed to output to disk" << endl;
        return -1;
    }
    output.close();
    
    // Verify that we are able to read the record back in
    Message readFromDisk;
    fstream input(outPath, ios::in | ios::binary);
    if (!readFromDisk.ParseFromIstream(&input)) {
        cerr << "Failed to read from disk" << endl;
        return -1;
    }
    input.close();
    
    // Validate all values
    assert(readFromDisk.sourcenodeid() == uuidFrom);
    assert(readFromDisk.destnodeid() == uuidTo);
    assert(readFromDisk.lastreceivedmsg() == 0);
    assert(readFromDisk.channelstate() == Message::ChannelState::Message_ChannelState_INITIAL_STARTUP);
    assert(readFromDisk.messageid() == 1);
    assert(readFromDisk.messagetype() == Message::MessageType::Message_MessageType_INFORMATION);
    assert(readFromDisk.has_info());
    assert(!readFromDisk.has_keyspace());
    assert(readFromDisk.info().records_size() == 2);
    
    cout << "All informational assertions passed!" << endl;
    
    // Add collection information record(s) to message
    toKeyspaceMessage(
        msg,
        {
            KeyspaceExchangeRecord{"test", 0, 4, 1},
            KeyspaceExchangeRecord{"test2", 8, 16, 2},
        }
    );
    
    // Output buffer to disk
    output.open(outPath, ios::out | ios::trunc | ios::binary);
    if (!msg.SerializeToOstream(&output)) {
        cerr << "Failed to output to disk" << endl;
        return -1;
    }
    output.close();
    
    // Verify that we are able to read the record back in
    input.open(outPath, ios::in | ios::binary);
    if (!readFromDisk.ParseFromIstream(&input)) {
        cerr << "Failed to read from disk" << endl;
        return -1;
    }
    input.close();
    
    // Validate all values
    assert(readFromDisk.sourcenodeid() == uuidFrom);
    assert(readFromDisk.destnodeid() == uuidTo);
    assert(readFromDisk.lastreceivedmsg() == 0);
    assert(readFromDisk.channelstate() == Message::ChannelState::Message_ChannelState_INITIAL_STARTUP);
    assert(readFromDisk.messageid() == 1);
    assert(readFromDisk.messagetype() == Message::MessageType::Message_MessageType_KEYSPACE);
    assert(readFromDisk.has_keyspace());
    assert(!readFromDisk.has_info());
    assert(readFromDisk.keyspace().keyspaces_size() == 2);
    
    cout << "All keyspace exchange assertions passed!" << endl;

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
