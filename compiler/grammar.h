#pragma once
#ifndef SPLC_GRAMMAR
#define NODE_GRAMMAR

#include <string>
#include <memory>
#include <cassert>
#include "node.h"

enum class ContainerType {
    Specifier = 0,
    Scope,
};

enum class DataType;

class Container {
private:
    const Node *node;
    const ContainerType type;
public:
    Container(const Node *node, const ContainerType type) : node(node), type(type) {};

    virtual ~Container() = default;

    virtual void installChild(std::vector<Node *>) = 0;

    const std::string &getTokenName();

    static std::shared_ptr<Container> generateContainer(const Node *node);

    template<typename T>
    std::shared_ptr<T> castTo() {
        static_assert(std::is_base_of<Container, T>::value, "T should inherit from Container");
        const ContainerType cType = T::containerType;
        assert(this->node->info.get() == this);
        assert(this->type == cType && "Type Casting Failed");
        return std::dynamic_pointer_cast<T>(this->node->info);
    }
};

class Scope : public Container {
public:
    static const ContainerType containerType = ContainerType::Scope;
    explicit Scope(Node * node) : Container(node, containerType) {}
    ~Scope() override = default;
    void installChild(std::vector<Node *>) override;
};

#endif