#ifndef SPLC_TYPE
#define SPLC_TYPE
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include "Container.h"

class Def;

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
    static const ContainerType containerType = ContainerType::Specifier;
    BasicType type;
    // for structure
    std::string structName; // structure identifer
    std::unique_ptr<std::vector<std::shared_ptr<Def>>> structDefList;
    // for pointer
    std::shared_ptr<Specifier> pointTo;

    explicit Specifier(const Node * node);
    Specifier(const Specifier& copy);
    Specifier(const Specifier&&) = delete;
    ~Specifier() override = default;
    void installChild(const std::vector<Node *>& children) override;

    void parseStruct(Node *pNode);
};
#endif