#pragma once
#ifndef SPLC_GRAMMAR
#define NODE_GRAMMAR

#include <string>
#include <memory>

class Node;
enum class DataType;

class Container {
public:
    Container() = default;
    virtual ~Container() = default;
    virtual void installChild(std::vector<Node *>) = 0;
    static std::shared_ptr<Container> generateContainer(const std::string &tokenName, DataType type, const std::string &data);
};

class Scope : public Container {
    ~Scope() override = default;
public:
    void installChild(std::vector<Node *>) override;
};

#endif