#include <chrono>
#include <fstream>
#include <iostream>
#include "message.hpp"

using namespace std;

int main(int argc, char **argv) {
    // Verify that protobuf version imported matches locally installed version
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Message msg = generateMessage();

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
