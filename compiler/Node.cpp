#include "Node.h"
#include "Container.h"
#include <utility>

void Node::printTree(Node *root, std::ostream& outputStream) {
    recursivePrint(root, 0, outputStream);
}

void Node::recursivePrint(Node *cur, int depth, std::ostream& outputStream) {
    if (cur->children.empty() && cur->type == DataType::PROD) return;
    if (cur->children.empty()) {
        for (int j = 0; j < depth; j++) {
            outputStream << "  ";
        }
        if (cur->type == DataType::DTYPE) {
            outputStream << "TYPE: " << cur->data << std::endl;
        } else if (cur->type == DataType::ID) {
            outputStream << "ID: " << cur->data << std::endl;
        } else if (cur->type == DataType::INT) {
            outputStream << "INT: " << cur->data << std::endl;
        } else if (cur->type == DataType::FLOAT) {
            outputStream << "FLOAT: " << cur->data << std::endl;
        } else if (cur->type == DataType::CHAR) {
            outputStream << "CHAR: " << cur->data << std::endl;
        } else if (cur->type == DataType::STRING) {
            outputStream << "STRING: " << cur->data << std::endl;
        } else {
            outputStream << cur->tokenName << std::endl;
        }
        return;
    } else {
        for (int j = 0; j < depth; j++)
            outputStream << "  ";
        outputStream << cur->tokenName << " (" << cur->lineno << ")" << std::endl;
        for (auto &i: cur->children) {
            recursivePrint(i, depth + 1, outputStream);
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
    if (parent->container) {
        parent->container->installChild(std::vector(childList));
    }
    return parent;
}

Node::Node(std::string tokenName, int lineno, DataType type, std::string data)
        : tokenName(std::move(tokenName)), lineno(lineno), type(type), data(std::move(data)) {
    container = Container::generateContainer(this);
}

