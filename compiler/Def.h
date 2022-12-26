#ifndef SPLC_DEF_H
#define SPLC_DEF_H

#include "Container.h"
#include "Specifier.h"
#include "Dec.h"

// Def instance could be ExtDef and Def in grammar
class Def : public Container {
public:
    explicit Def(Node* node);
    Def(const Def& defList) = default;
    Def(const Def&& defList) = delete;
    ~Def() override = default;
    void installChild(const std::vector<Node *>& children) override;

    std::shared_ptr<Specifier> specifier;
    std::vector<std::shared_ptr<Dec>> declares;
private:
    void parseDecList();

    void parseExtDecList();
};


#endif //SPLC_DEF_H
