#include "UUID.h"

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

    for(int i = 0; i < TRUNCATED_UUID; i++){
        strUUID += sha_uuid[i];
    }
    cout << "Returning: " << UUIDToHex(strUUID) << endl;
    return UUIDToHex(strUUID);
}

std::string UUIDToHex(UUID uuid, bool upper) {
    std::ostringstream ret;
    unsigned int c;
    for (std::string::size_type i = 0; i < uuid.length(); ++i) {
        c = (unsigned int)(unsigned char)uuid.at(i);
        ret << std::hex << std::setfill('0') <<
            std::setw(2) << (upper ? std::uppercase : std::nouppercase) << c;
    }
    return ret.str();
}
