#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <vector>
#include <initializer_list>

using std::string;
using std::vector;
using std::initializer_list;

enum class DataType
{
    INT,
    FLOAT,
    CHAR,
    DTYPE,
    ID,
    OTHER       // keyword, operator and other symbols
};

class Node
{
public:

    string token_name{};
    string data{};
    DataType type{};
    int lineno{};
    Node *parent;
    vector<Node *> children{};

    Node(string token_name, int lineno, DataType type, string data="")
        : token_name(token_name), lineno(lineno), type(type), data(data)
    {}

    void set_children(initializer_list<Node *> child_list)
    {
        for (Node *child : child_list)
        {
            child->parent = this;
            children.push_back(child);
        }
    }
};

#endif