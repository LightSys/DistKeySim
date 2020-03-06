#ifndef ADAK_KEYING_KEYSPACE_H
#define ADAK_KEYING_KEYSPACE_H

typedef unsigned long ADAK_Key_t;
class Keyspace {
private:
    /**
     * Each keyspace block is designated by S-E/B, start is the S, end is the E, and suffix is B
     */
    uint32_t start;
    uint32_t end;
    uint32_t suffix;
public:
    Keyspace(unsigned long start, unsigned long end, unsigned long suffix);

    Keyspace(const Keyspace &obj);
    ~Keyspace() = default;
    
    /**
     * Uses one the key based on the available keyspace block and moves the start variable to the next position.
     * This allows the option to call getNextAvailableKey over and over to use up the keyspace
     * @return Next available 32 bit key
     */
    ADAK_Key_t getNextAvailableKey();

    /**
     * This returns the same information that getNextAvailableKey does, but it doens't actually use up they keyspace.
     * This is for implementing the ADAK algorithm specification: Find the lowest-numbered starting
     * number S in the list of blocks/subblocks assigned to the node for the given database table.
     * @return
     */
    // ADAK_Key_t checkNextAvailableKey();

    // Getters
    uint32_t getStart() const { return this->start; }
    uint32_t getEnd() const { return this->end; }
    uint32_t getSuffix() const { return this->suffix; }
    
    constexpr inline bool isKeyAvailable() const {
        return start < end;
    }
};
#endif //ADAK_KEYING_KEYSPACE_H
