#ifndef LIGHTSYS_ADAK_UUID_H
#define LIGHTSYS_ADAK_UUID_H

#include <random>
#include <array>
#include <string>
#include <iomanip>

#include "picoSHA2.h"

// random 256 bit uuid type:
typedef std::array<u_char, picosha2::k_digest_size> uuid_256;
static std::random_device uuid_random_bits;

//typedef std::array<u_char, 16> uuid_t;
typedef std::string HexDigest;
typedef std::string UUID;

const unsigned int NUM_SEED_BYTES = 16;
const unsigned int TRUNCATED_UUID = 16;

HexDigest new_uuid();
std::string UUIDToHex(HexDigest uuid, bool upper = false);

#endif // LIGHTSYS_ADAK_UUID_H