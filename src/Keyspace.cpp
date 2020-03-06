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
    this->suffix = obj.start;
}


adak_key Keyspace::getNextAvailableKey() {
    int skipBy = pow(2, this->suffix);
    unsigned long oldStart = this->start;
    this->start = start + skipBy;

    if (start >= end) {
        // This means it is out of keyspace
        cout << "ERROR start == end on Keyspace::getNextAvailableKey()" << endl;
    }
    return oldStart;
}

bool Keyspace::isKeyAvailable() {
    return start <= end;
}