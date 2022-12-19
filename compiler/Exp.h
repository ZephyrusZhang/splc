#ifndef SPLC_EXP_H
#define SPLC_EXP_H
#include "Container.h"
#include "Node.h"
#include "CompoundType.h"

enum class ValueType {
    Unknown,
    LValue,
    RValue
};

class Exp : public Container {
private:
    ValueType valueType = ValueType::Unknown;
    std::shared_ptr<CompoundType> expCompoundType;
    int integerValue;
public:
    const static ContainerType containerType = ContainerType::Exp;
    ExpType expType;
    explicit Exp(Node *node, ExpType expType);
    ~Exp() override = default;

    [[nodiscard]] const CompoundType &getCompoundType() const;

    [[nodiscard]] ValueType getValueType() const {
        return this->valueType;
    }

    [[nodiscard]] std::shared_ptr<Exp> getChildExp(const size_t idx) const {
        assert(idx < node->children.size());
        return node->children[idx]->container->castTo<Exp>();
    }

    void installChild(const std::vector<Node *> &vector) override;
    [[nodiscard]] int getIntegerValue() const {
        return this->integerValue;
    }
};


#endif //SPLC_EXP_H
