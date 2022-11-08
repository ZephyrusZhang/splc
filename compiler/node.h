#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <initializer_list>
#include "type.h"

extern std::ofstream outputFile;
extern int errCount;

enum class DataType {
    INT,
    FLOAT,
    CHAR,
    STRING,
    DTYPE,      // data type
    ID,
    OTHER,      // keyword, operator and other symbols
    PROD,
    ERR
};

class Symbol {
public:

};

class Container {
public:
    virtual ~Container() = 0;

    Container() = default;

    static Container *generateContainer(const std::string &tokenName, DataType type, const std::string &data) {
        return nullptr;
    }
};

class Scope : public Container {
//    public:
//  /  std::map<std::string,
};

class Node {
public:

    const std::string tokenName;
    const std::string data;
    DataType type;
    int lineno = 0;
    Node *parent = nullptr;
    Container *info = nullptr;
    std::vector<Node *> children;

    explicit Node() = delete;

    Node(const std::string &tokenName, int lineno, DataType type, const std::string &data = "")
            : tokenName(tokenName), lineno(lineno), type(type), data(data) {
        info = Container::generateContainer(tokenName, type, data);
    }

    static Node *createNodeWithChildren(const std::string &tokenName, int lineno, DataType type,
                                        std::initializer_list<Node *> childList, const std::string &data = "") {
        Node *parent = new Node(tokenName, lineno, type, data);
        for (Node *child: childList) {
            child->parent = parent;
            parent->children.push_back(child);
        }

        return parent;
    }

    static void printTree(Node *root) {
        recursivePrint(root, 0);
    }

private:
    static void recursivePrint(Node *cur, int depth) {
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
};


#endif