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
    
    // Verify that we are able to read the record back in
    Message readFromDisk;
    fstream input(outPath, ios::in | ios::binary);
    if (!readFromDisk.ParseFromIstream(&input)) {
        cerr << "Failed to read from disk" << endl;
        return -1;
    }
    
    // Validate all values
    assert(readFromDisk.);

    // Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
