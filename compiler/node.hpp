#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <cstring>
#include <memory>
#include <vector>
#include <initializer_list>
#include <stdio.h>


#define DEBUG_MARCRO printf("Fuck\n");

std::string output_path = "";
FILE *output_file;
int err_count = 0;

enum class DataType
{
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

class Container {
    public:
    virtual ~Container() = 0;
    Container() = default;
    static Container* generateContainer(const std::string& token_name, DataType type, const std::string& data) {
        return nullptr;
    }
};

class Node
{
public:

    const std::string token_name{};
    const std::string data{};
    DataType type{};
    int lineno = 0;
    Node* parent = nullptr;
    Container* info = nullptr;
    std::vector<Node*> children;

    explicit Node() = delete;

    Node(std::string token_name, int lineno, DataType type, std::string data="")
        : token_name(token_name), lineno(lineno), type(type), data(data)
    {
        info = Container::generateContainer(token_name, type, data);
    }

    static Node* create_node_with_children(std::string token_name, int lineno, DataType type, std::initializer_list<Node *> child_list, std::string data="")
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
        // if (cur == nullptr) return;
        if (cur->children.size() == 0 && cur->type == DataType::PROD)
        {
            return;
        }

        if (cur->children.size() == 0)
        {
            for (int j = 0; j < depth; j++)
            {
                std::cout << "  ";
            }
            if (cur->type == DataType::DTYPE)
            {
                 std::cout << ("TYPE: " + cur->data) << std::endl;
            }
            else if (cur->type == DataType::ID)
            {
                std::cout << ("ID: " + cur->data) << std::endl;
            }
            else if (cur->type == DataType::INT)
            {
                std::cout << ("INT: " + cur->data) << std::endl;
            }
            else if (cur->type == DataType::FLOAT)
            {
                std::cout << ("FLOAT: " + cur->data) << std::endl;
            }
            else if (cur->type == DataType::CHAR)
            {
                std::cout << ("CHAR: " + cur->data) << std::endl;
            }
            else if (cur->type == DataType::STRING)
            {
                std::cout << ("STRING: " + cur->data) << std::endl;
            }
            else
            {
                
                std::cout << cur->token_name << std::endl;
                
            }
            return;
        }

        else
        {
            for (int j = 0; j < depth; j++)
            {
                    std::cout << "  ";
            }
            std::cout << (cur->token_name  + " " + "(" + std::to_string(cur->lineno) + ")") <<std:: endl;
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

void init_args(std::string file_name)
{
    int index = file_name.find_last_of('.');
    output_path = file_name.substr(0, index + 1) + "out";

    printf("%s\n", output_path.c_str());

    // if (output_path == "")
    // {
    //     std::system("mkdir -p out");
    //     output_path = "./out/" + output_file_name;
    // }

    output_file = fopen(output_path.c_str(), "w+");
}

std::string get_file_name(std::string input_file_path)
{
    int index = input_file_path.find_last_of('/');
    return input_file_path.substr(index + 1);
}

#endif