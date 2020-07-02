#include <iostream>
#include <cmath>
#include <climits>

#include "Keyspace.h"

using namespace std;

Keyspace::Keyspace(unsigned long start, unsigned long end, unsigned long suffix)
    : start(start), end(end), suffix(suffix)
{
    if(this->end == UINT_MAX){
       this->percent = 1.0/pow(2, this->suffix);
    } else{
       this->percent = 0;
    }
    
}

Keyspace::Keyspace(const Keyspace &obj){
    this->start = obj.start;
    this->end = obj.end;
    this->suffix = obj.suffix;
    if(obj.end == UINT_MAX){
       this->percent = 1.0/pow(2, obj.suffix);
    } else{
       this->percent = 0;
    }
//cout << "just created keyspace with suffix " << this->suffix << " and % " << this->percent << endl; 
}

ADAK_Key_t Keyspace::getNextAvailableKey() {
    // Increment Keyspace unused start by 2^suffix, returning previous start point as an available key
    unsigned long skipBy = 1.0/this->percent;
    unsigned long oldStart = this->start;

    if (start >= end) {
        // This means it is out of keyspace
        throw KeyspaceException("start >= end when creating new key");
    }
    this->start = start + skipBy;
    return oldStart;
}

ADAK_Key_t Keyspace::getSize(){
    ADAK_Key_t size = (this->end - this->start)>>this->suffix;//using bitshifting to calculate the size of the keyspace 
    return size;
} 


bool Keyspace::operator==(const Keyspace & rhs){
   return suffix == rhs.getSuffix() && start == rhs.getStart() && end == rhs.getEnd();
}
