#pragma once
#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <initializer_list>
#include "grammar.h"

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
    Node(std::string tokenName, int lineno, DataType type, std::string data = "");
    static Node *createNodeWithChildren(const std::string &tokenName, int lineno, DataType type, std::initializer_list<Node *> childList, const std::string &data = "");
    static void printTree(Node *root);
private:
    static void recursivePrint(Node *cur, int depth);
};


#endif