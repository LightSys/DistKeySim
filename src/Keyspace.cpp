#include <iostream>
#include <math.h>
#include "Keyspace.h"

using namespace std;
Keyspace::Keyspace(unsigned long start, unsigned long end, unsigned long suffix) : start(start), end(end), suffix(suffix) {
    cout << "Creating Keyspace " << start << " - " <<  end << " / " << suffix << endl;
}

adak_key Keyspace::checkNextAvailableKey() {
    int skipBy = pow(this->suffix, 2);
    unsigned long oldStart = this->start;
    this->start = start + skipBy;
    if(start != end) {
        cout << "Keyspace is empty!" << endl;
    }
    return oldStart;
}

adak_key Keyspace::getNextAvailableKey() {
    int skipBy = pow(this->suffix, 2);
    unsigned long oldStart = this->start;
    this->start = start + skipBy;
    if(start != end) {
        cout << "ERROR start == end on Keyspace::getNextAvailableKey()" << endl;
    }
    return oldStart;
}
