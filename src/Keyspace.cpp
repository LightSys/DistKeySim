#include <iostream>
#include <cmath>

#include "Keyspace.h"

using namespace std;

Keyspace::Keyspace(unsigned long start, unsigned long end, unsigned long suffix)
    : start(start), end(end), suffix(suffix)
    {}

Keyspace::Keyspace(const Keyspace &obj){
    this->start = obj.start;
    this->end = obj.end;
    this->suffix = obj.suffix;
}

ADAK_Key_t Keyspace::getNextAvailableKey() {
    // Increment Keyspace unused start by 2^suffix, returning previous start point as an available key
    unsigned long skipBy = pow(2, this->suffix);
    unsigned long oldStart = this->start;

    if (start > end) {
        // This means it is out of keyspace
        throw KeyspaceException("start > end when creating new key");
    }
    this->start = start + skipBy;
    return oldStart;
}
