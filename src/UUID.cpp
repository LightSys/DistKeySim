#include "UUID.h"

using namespace std;

HexDigest new_uuid() {
    // generate seed bits:
    std::array<unsigned char, NUM_SEED_BYTES> seed{};
    for (u_char &ch : seed){
        ch = uuid_random_bits();
    }

    // generate sha256 hash:
    uuid_256 sha_uuid;
    picosha2::hash256(seed, sha_uuid);

    // added to original function to return a usable truncated ID and returns a string
    HexDigest strUUID;

    //TODO::NEEDS TO BE TESTED once working
    for(int i = 0; i < TRUNCATED_UUID; i++){
        strUUID += sha_uuid[i];
    }
    return strUUID;
}

string UUIDToHex(HexDigest uuid, bool upper) {
    ostringstream ret;
    for (u_int ch : uuid) {
        ret << hex << setfill('0') <<
            setw(2) << (upper ? uppercase : nouppercase) << ch;
    }
    return ret.str();
}