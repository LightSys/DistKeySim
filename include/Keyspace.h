#ifndef ADAK_KEYING_KEYSPACE_H
#define ADAK_KEYING_KEYSPACE_H

typedef unsigned long adak_key;
class Keyspace {
private:
    /**
     * Each keyspace block is designated by S-E/B, start is the S, end is the E, and suffix is B
     */
    unsigned long start;
    unsigned long end;
    unsigned long suffix;
public:
    Keyspace(unsigned long start, unsigned long end, unsigned long suffix);

    Keyspace(const Keyspace &obj);
    ~Keyspace() = default;
    /**
     * Uses one the key based on the available keyspace block and moves the start variable to the next position.
     * This allows the option to call getNextAvailableKey over and over to use up the keyspace
     * @return
     */
    adak_key getNextAvailableKey();
    /**
     * This returns the same information that getNextAvailableKey does, but it doens't actually use up they keyspace.
     * This is for implementing the ADAK algorithm specification: Find the lowest-numbered starting
     * number S in the list of blocks/subblocks assigned to the node for the given database table.
     * @return
     */
//    adak_key checkNextAvailableKey();

    // Getters
    unsigned long getStart() const { return this->start; }
    unsigned long getEnd() const { return this->end; }
    unsigned long getSuffix() const { return this->suffix; }

    bool isKeyAvailable();
};
#endif //ADAK_KEYING_KEYSPACE_H