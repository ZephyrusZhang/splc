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
enum class ExpType {
    ASSIGN = 0,
    AND,
    OR,
    LT,
    LE,
    GT,
    GE,
    NE,
    EQ,
    PLUS,
    INCREASE,
    DECREASE,
    NEGATIVE_SIGN,
    MINUS,
    MUL,
    DIV,
    SCOPE,
    NOT,
    FUNC_INVOKE,
    ARRAY_INDEX,
    DOT_ACCESS,
    PTR_ACCESS,
    IDENTIFIER,
    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_CHAR,
    LITERAL_STRING,
    ADDRESS_OF,
    TYPE_CAST,
    DEREF
};

enum class StmtType
{
    SINGLE = 0,
    COMP,
    RETURN,
    CONTINUE,
    BREAK,
    IF,
    IF_ELSE,
    WHILE,
    FOR
};

class Container;
class Scope;
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

    bool hasToken(std::initializer_list<const std::string> acceptItemsToken) const;

    static Node *createNodeWithChildren(const std::string &tokenName, int lineno, DataType type, std::initializer_list<Node *> childList, const std::string &data = "");
    static Node *createExpNodeWithChildren(const std::string &tokenName, int lineno, ExpType expType, std::initializer_list<Node *> childList);
    static Node *createStmtNodeWithChildren(const std::string &tokenName, int lineno, StmtType stmtType, std::initializer_list<Node *> childlist);
    static void printTree(const Node *root, std::ostream& outputStream = outputFile);
    static std::vector<Node *> convertTreeToVector(const Node * root, const std::string& recursiveName, std::initializer_list<const std::string> acceptItemsToken, bool anyRecursiveName=false);
private:
    static void recursivePrint(const Node *cur, int depth, std::ostream& outputStream);
};


#endif