/*
 * This is an example source file.
 *
*/

#include "node.h"
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char **argv){

    srand(time(NULL));
    vector<Node*> nodeList;

    // Create some nodes and connect them
    for(int i = 0; i < 10; i++) {
        Node* node = new Node(i, nodeList);
        nodeList.push_back(node);
    }
    nodeList.at(0)->removeNodeAt(2);
    nodeList.at(0)->sendMessage("Hello", 3);

    return 0;
}
