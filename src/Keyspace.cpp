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


adak_key Keyspace::getNextAvailableKey() {
    unsigned long skipBy = pow(2, this->suffix);
    unsigned long oldStart = this->start;
    this->start = start + skipBy;

    if (start > end) {
        // This means it is out of keyspace
        cout << "ERROR start > end on Keyspace::getNextAvailableKey()" << endl;
    }
    return oldStart;
}

bool Keyspace::isKeyAvailable() {
    return start <= end;
}
bool Keyspace::isAnotherKeyAvailable() {
    int newStart = this->start + pow(2, this->suffix);
    return newStart <= this->end;
}