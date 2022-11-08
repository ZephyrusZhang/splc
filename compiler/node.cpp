#include "node.h"
#include "grammar.h"
#include <utility>

void Node::printTree(Node *root) {
    recursivePrint(root, 0);
}

void Node::recursivePrint(Node *cur, int depth) {
    if (cur->children.empty() && cur->type == DataType::PROD) return;
    if (cur->children.empty()) {
        for (int j = 0; j < depth; j++) {
            outputFile << "  ";
        }
        if (cur->type == DataType::DTYPE) {
            outputFile << "TYPE: " << cur->data << std::endl;
        } else if (cur->type == DataType::ID) {
            outputFile << "ID: " << cur->data << std::endl;
        } else if (cur->type == DataType::INT) {
            outputFile << "INT: " << cur->data << std::endl;
        } else if (cur->type == DataType::FLOAT) {
            outputFile << "FLOAT: " << cur->data << std::endl;
        } else if (cur->type == DataType::CHAR) {
            outputFile << "CHAR: " << cur->data << std::endl;
        } else if (cur->type == DataType::STRING) {
            outputFile << "STRING: " << cur->data << std::endl;
        } else {
            outputFile << cur->tokenName << std::endl;
        }
        return;
    } else {
        for (int j = 0; j < depth; j++)
            outputFile << "  ";
        outputFile << cur->tokenName << " (" << cur->lineno << ")" << std::endl;
        for (auto &i: cur->children) {
            recursivePrint(i, depth + 1);
        }
    }
}

Node *Node::createNodeWithChildren(const std::string &tokenName, int lineno, DataType type,
                                   std::initializer_list<Node *> childList, const std::string &data) {
    Node *parent = new Node(tokenName, lineno, type, data);
    for (Node *child: childList) {
        child->parent = parent;
        parent->children.push_back(child);
    }

    return parent;
}

Node::Node(std::string tokenName, int lineno, DataType type, std::string data)
        : tokenName(std::move(tokenName)), lineno(lineno), type(type), data(std::move(data)) {
    info = Container::generateContainer(this->tokenName, type, this->data);
}

