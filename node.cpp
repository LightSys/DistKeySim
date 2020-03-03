//
// Created by Nathan O'Neel on 2/15/20.
//
#include "node.h"
#include <iostream>

using namespace std;

Node::Node(const int id, vector<Node*> nodeList) : id(id), nodeList(nodeList) {
    // Go through current nodeList and add myself it it
    for(auto node : nodeList) {
        node->addNodeToNodeList(this);
    }
}

/**
 * @param message
 * @param nodeId
 */
void Node::sendMessage(string message, int nodeId) {
    if(id == nodeId) {
        cout << "ID: " << id << " Received Message: " << message << endl;
    } else {
        Node* node = getNodeById(nodeId);
        if(node == nullptr) {
            node = getRandomNode();
            cout << "Picking random node: " << node->getId() << endl;
        }
        cout << "To: " << node->getId() << " From: " << id << " Message: " << message << endl;
        node->sendMessage(message, nodeId);
    }
}

void Node::addNodeToNodeList(Node* node) {
    nodeList.push_back(node);
}

Node* Node::getNodeById(int id) {
    for(int i = 0; i < nodeList.size(); i++) {
        if(nodeList.at(i)->getId() == id) {
            return nodeList.at(i);
        }
    }
    return nullptr;
}

Node* Node::getRandomNode() {
    return nodeList.at(rand() % nodeList.size() + 1);
}
