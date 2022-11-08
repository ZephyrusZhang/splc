#ifndef SPLC_TYPE
#define SPLC_TYPE
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include "grammar.h"

enum BasicType {
    TypeUnknown,
    TypeVoid,
    TypeChar,
    TypeInt,
    TypeFloat,
    TypeStruct,
    TypePointer
};

class Specifier : public Container {
public:
    const std::string name;
    BasicType type;
    std::string structName;
    std::shared_ptr<Specifier> pointTo;

    Specifier(std::string name, const BasicType type) : name(std::move(name)), type(type) {
    }
    ~Specifier() override = default;
    void installChild(std::vector<Node *> children) override;

    void parseStruct(Node *pNode);
};
#endif