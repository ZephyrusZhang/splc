#ifndef SPLC_DEF_H
#define SPLC_DEF_H

#include "Container.h"
#include "Specifier.h"
#include "Dec.h"

class Def : public Container {
public:
    const static ContainerType containerType = ContainerType::DefList;
    explicit Def(const Node* node);
    Def(const Def& defList) = default;
    Def(const Def&& defList) = delete;
    ~Def() override = default;
    void installChild(std::vector<Node *> vector) override;

    std::shared_ptr<Specifier> specifier;
    std::vector<std::shared_ptr<Dec>> declares;
private:
    void parseDecList();
};


#endif //SPLC_DEF_H
