#pragma once
#ifndef SPLC_GRAMMAR
#define NODE_GRAMMAR

#include <string>
#include <memory>
#include <cassert>
#include "Node.h"

enum class ContainerType {
    Specifier = 0,
    Scope,
    DefList,
    Dec,
    Exp
};

enum class DataType;

class Container {
protected:
    const ContainerType type;
public:
    Node *node;
    Container(Node *node, const ContainerType type) : node(node), type(type) {};
    virtual ~Container() = default;
    virtual void installChild(const std::vector<Node *> &) = 0;
    virtual void onThisInstalled() {};

    ContainerType getContainerType() { return this->type; }
    [[nodiscard]] const std::string &getTokenName() const noexcept;
    static void generateContainer(Node *node);

    template<typename T>
    std::shared_ptr<T> castTo() {
        static_assert(std::is_base_of<Container, T>::value, "T should inherit from Container");
        const ContainerType cType = T::containerType;
        assert(this->node->container);
        assert(this->node->container.get() == this);
        assert(this->type == cType && "Type Casting Failed");
        return std::dynamic_pointer_cast<T>(this->node->container);
    }
};

#endif