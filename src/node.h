//
// Created by Nathan O'Neel on 2/15/20.
//
#ifndef LIGHTSYS_ADAK_NODE_H
#define LIGHTSYS_ADAK_NODE_H

#include <string>
#include <iostream>
#include <vector>

class Node {
private:
    int id;
    std::vector<Node*> nodeList;

public:
    Node(const int id, std::vector<Node*> nodeList);

    void sendMessage(std::string message, int nodeId);

    void addNodeToNodeList(Node* node);

    // Printing
    void printNodeList() {
        for(auto node : nodeList) {
            std::cout << node->getId() << " ";
        }
        std::cout << std::endl;
    }

    void removeNodeAt(int index) { nodeList.erase(nodeList.begin() + index); }

    // Getters
    int getId() { return id; };
    Node* getNodeById(int id);
    Node* getRandomNode();

    // Setters
    void setId(int newId) { id = newId; };
};

#endif //LIGHTSYS_ADAK_NODE_H
