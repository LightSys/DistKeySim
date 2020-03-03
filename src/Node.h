#ifndef LIGHTSYSTEMP_NODE_H
#define LIGHTSYSTEMP_NODE_H

#include <iostream>
#include <list>

///TODO integrate RPC into the class
///TODO likely other things that I didn't consider.
class Node {
private:
    std::string UUID;
    int keySpace;
    int keyGenRate;
    int keyShareRate;
    bool active;
    std::list<Node> peers; ///called directConnection on the board.

public:
    Node();

    /**
     * Creates the keyspace for the Node
     * @return
     */
    int createKeySpace();

    // Getters
    std::string getUUID() const { return UUID; }
    int getKeySpace() const { return keySpace; }
    int getKeyGenRate() const { return keyGenRate; }
    int getKeyShareRate() const { return keyShareRate; }
    bool getActive() const { return active; }

    // Setters
    void setNodeID(std::string newUUID) { UUID = newUUID; }
    void setKeySpace(int ks) { keySpace = ks; }
    void setKeyGenRate(int kgr) { keyGenRate = kgr; }
    void setKeyShareRate(int ksr) { keyShareRate = ksr; }
    void setActive(bool a) { active = a; }


    // Printing
    void printUUID();
};


#endif //LIGHTSYSTEMP_NODE_H
