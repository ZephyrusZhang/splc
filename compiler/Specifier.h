#ifndef SPLC_TYPE
#define SPLC_TYPE
#include <string>
#include <utility>
#include <memory>
#include <vector>
#include "Container.h"

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
    std::string structName; // for structure
    std::shared_ptr<Specifier> pointTo; // for pointer

    explicit Specifier(const Node * node) : Container(node, containerType) {
        type = TypeUnknown;
    }
    ~Specifier() override = default;
    void installChild(std::vector<Node *> children) override;

    void parseStruct(Node *pNode);
};
#endif