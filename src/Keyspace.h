//
// Created by Nathan O'Neel on 3/3/20.
//

#ifndef ADAK_KEYING_KEYSPACE_H
#define ADAK_KEYING_KEYSPACE_H


class Keyspace {
private:
    unsigned long start;
    unsigned long end;
    unsigned long offset;
public:
    Keyspace(unsigned long start, unsigned long end, unsigned long offset);

    unsigned long getNextAvailableKey();

    // Getters
    unsigned long getStart() const { return this->start; }
    unsigned long getEnd() const { return this->end; }
    unsigned long getOffset() const { return this->offset; }
};


#endif //ADAK_KEYING_KEYSPACE_H
