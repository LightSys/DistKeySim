#include <iostream>
#include <math.h>
#include "Keyspace.h"

using namespace std;
Keyspace::Keyspace(unsigned long start, unsigned long end, unsigned long offset) : start(start), end(end), offset(offset) {
    cout << "Creating Keyspace " << start << " - " <<  end << " / " << offset << endl;
}


unsigned long Keyspace::getNextAvailableKey() {
    int skipBy = pow(offset, 2);
    unsigned long oldStart = this->start;
    this->start = start + skipBy;
    if(start != end) {
        cout << "ERROR start == end on Keyspace::getNextAvailableKey()" << endl;
    }
    return oldStart;
}
