#ifndef SPLC_EXP_H
#define SPLC_EXP_H
#include "Container.h"
#include "Node.h"

class Exp : public Container {
public:
    const static ContainerType containerType = ContainerType::Exp;
    ExpType expType;
    explicit Exp(Node *node, ExpType expType);
    ~Exp() override = default;

    void installChild(const std::vector<Node *> &vector) override;
};


#endif //SPLC_EXP_H
