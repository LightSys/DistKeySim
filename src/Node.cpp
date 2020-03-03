#include <iostream>
#include "Node.h"
#include "UUID.h"

using namespace std;


Node::Node() {
    // Create UUID
    this->UUID = new_uuid();
}
///creates the key space for the Node
int Node::createKeySpace() {

}

void Node::printUUID() {
    cout << this->UUID << endl;
}