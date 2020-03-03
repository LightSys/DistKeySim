#include "UUID.h"

using namespace std;

UUID new_uuid() {
    // generate seed bits:
    std::array<unsigned char, NUM_SEED_BYTES> seed;
    for(auto ch = seed.begin(); ch != seed.end(); ++ch){
        *ch = uuid_random_bits();
    }

    // generate sha256 hash:
    uuid_t sha_uuid;
    picosha2::hash256(seed, sha_uuid);

    // added to original function to return a usable truncated ID and returns a string
    UUID strUUID;

    //TODO::NEEDS TO BE TESTED once working
    for(int i = 0; i < TRUNCATED_UUID; i++){
        strUUID += sha_uuid[i];
    }
    return strUUID;
}

string UUIDToHex(UUID uuid, bool upper) {
    ostringstream ret;
    unsigned int c;
    for (string::size_type i = 0; i < uuid.length(); ++i) {
        c = (unsigned int)(unsigned char)uuid.at(i);
        ret << hex << setfill('0') <<
            setw(2) << (upper ? uppercase : nouppercase) << c;
    }
    return ret.str();
}