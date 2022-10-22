#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <vector>
#include <initializer_list>
#include <stdio.h>

using std::string;
using std::vector;
using std::initializer_list;
using std::cout;
using std::endl;

#define DEBUG_MARCRO printf("Fuck\n");

string output_path = "";
FILE *output_file;
int err_count = 0;

enum class DataType
{
    INT,
    FLOAT,
    CHAR,
    DTYPE,      // data type
    ID,
    OTHER,      // keyword, operator and other symbols
    PROD,
    ERR
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

    static Node *create_node_with_children(string token_name, int lineno, DataType type, initializer_list<Node *> child_list, string data="")
    {
        Node *parent = new Node(token_name, lineno, type, data);
        for (Node *child : child_list)
        {
            child->parent = parent;
            parent->children.push_back(child);
        }

        return parent;
    }

    static void print_tree(Node *root)
    {
        recursive_print(root, 0);
    }

private:
    static void recursive_print(Node *cur, int depth)
    {
        if (cur->children.size() == 0 && cur->type == DataType::PROD)
        {
            return;
        }

        if (cur->children.size() == 0)
        {
            for (int j = 0; j < depth; j++)
            {
                cout << "  ";
            }
            if (cur->type == DataType::DTYPE)
            {
                 cout << ("TYPE: " + cur->data) << endl;
            }
            else if (cur->type == DataType::ID)
            {
                cout << ("ID: " + cur->data) << endl;
            }
            else if (cur->type == DataType::INT)
            {
                cout << ("INT: " + cur->data) << endl;
            }
            else if (cur->type == DataType::FLOAT)
            {
                cout << ("FLOAT: " + cur->data) << endl;
            }
            else if (cur->type == DataType::CHAR)
            {
                cout << ("CHAR: " + cur->data) << endl;
            }
            else
            {
                
                cout << cur->token_name << endl;
                
            }
            return;
        }

        else
        {
            for (int j = 0; j < depth; j++)
            {
                    cout << "  ";
            }
            cout << (cur->token_name  + " " + "(" + std::to_string(cur->lineno) + ")") << endl;
            for (int i = 0; i < cur->children.size(); i++)
            {
                recursive_print(cur->children[i], depth + 1);
            }
        }
    }
};

void output_tree(Node *root)
{
    printf("Start to output to %s\n", output_path.c_str());
    freopen(output_path.c_str(), "w", stdout);
    Node::print_tree(root);
}

void init_args(string file_name)
{
    int index = file_name.find('.');
    string output_file_name = file_name.substr(0, index + 1) + "out";

    if (output_path == "")
    {
        std::system("mkdir -p out");
        output_path = "./out/" + output_file_name;
    }

    output_file = fopen(output_path.c_str(), "w+");
}

string get_file_name(string input_file_path)
{
    int index = input_file_path.find_last_of('/');
    return input_file_path.substr(index + 1);
}

#endif