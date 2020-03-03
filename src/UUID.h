#ifndef _UUID_H
#define _UUID_H

#include <random>
#include <array>
#include <string>
#include"picoSHA2.h"


// random 256 bit uuid type:
typedef std::array<unsigned char, picosha2::k_digest_size> uuid_t;
static std::random_device uuid_random_bits;

const unsigned int NUM_SEED_BYTES = 16;
const unsigned int TRUNCATED_UUID = 16;

std::string new_uuid(){
    //generate seed bits:
    std::array<unsigned char, NUM_SEED_BYTES> seed;
    for(auto ch = seed.begin(); ch != seed.end(); ++ch){
        *ch = uuid_random_bits();
    }

    // generate sha256 hash:
    uuid_t sha_uuid;
    picosha2::hash256(seed, sha_uuid);

    //added to orginal function to return a usable truncated ID and returns a string
    std::string strUUID;

    //TODO::NEEDS TO BE TESTED once working
    for(int i = 0; i < TRUNCATED_UUID; i++){
        strUUID += sha_uuid[i];
    }




    return strUUID;
}

#endif // _UUID_H