#include <vector>

#include "catch.hpp"
#include "Node.h"

using namespace std;

TEST_CASE("Multiple nodes have unique UUIDs", "[main]") {
    // Test that 10 unique Nodes will not have matching UUIDs
    int numNodes = 10;
    vector<Node> testNodes;
    
    for (int &&i = 0; i < numNodes; i++) {
        testNodes.emplace_back(Node());
    }
    
    for (int &&i = 0; i < numNodes; i++) {
        for (int &&j = i + 1; j < numNodes; j++) {
            REQUIRE(testNodes.at(i).getUUID() != testNodes.at(j).getUUID());
        }
    }
}

TEST_CASE("Single Node has minimum index of 0", "[main]") {
    Node testNode;
    REQUIRE(testNode.minimumKeyspaceIndex() == 0);
}