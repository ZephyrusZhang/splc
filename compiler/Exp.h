#ifndef SPLC_EXP_H
#define SPLC_EXP_H
#include "Container.h"
#include "Node.h"
#include "Specifier.h"

enum class ValueType {
    Unknown,
    LValue,
    RValue
};

class Exp : public Container {
private:
    ValueType valueType = ValueType::Unknown;
    std::shared_ptr<Specifier> expSpecifier;
public:
    const static ContainerType containerType = ContainerType::Exp;
    ExpType expType;
    explicit Exp(Node *node, ExpType expType);
    ~Exp() override = default;

    void installChild(const std::vector<Node *> &vector) override;
};


#endif //SPLC_EXP_H
