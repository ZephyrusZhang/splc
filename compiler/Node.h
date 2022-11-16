#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <initializer_list>

extern std::ofstream outputFile;
extern int errCount;

class Container;

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
class Container;
class Node {
public:
    const std::string tokenName;
    const std::string data;
    DataType type;
    int lineno;
    Node *parent = nullptr;
    std::shared_ptr<Container> container;
    std::vector<Node *> children;
    explicit Node() = delete;
    Node(std::string tokenName, int lineno, DataType type, std::string data = "");
    static Node *createNodeWithChildren(const std::string &tokenName, int lineno, DataType type, std::initializer_list<Node *> childList, const std::string &data = "");
    static void printTree(Node *root, std::ostream& outputStream = outputFile);
    static std::vector<Node *> convertTreeToVector(const Node * root, const std::string& recursiveName, std::initializer_list<const std::string> acceptItemsToken);
private:
    static void recursivePrint(Node *cur, int depth, std::ostream& outputStream);
};


#endif