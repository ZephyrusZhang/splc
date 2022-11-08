#pragma once
#ifndef SPLC_GRAMMAR
#define NODE_GRAMMAR

#include <string>
#include <memory>

enum class DataType;

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

#endif