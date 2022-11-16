#ifndef SPLC_SCOPE_H
#define SPLC_SCOPE_H

#include <vector>
#include "Node.h"
#include "Container.h"
#include "SymbolTable.h"

class Scope : public Container {
public:
    std::vector<SymbolTable> symbolTable;
    std::shared_ptr<Scope> parentScope;

    static const ContainerType containerType = ContainerType::Scope;
    explicit Scope(Node * node) : Container(node, containerType) {}
    ~Scope() override = default;
    void installChild(const std::vector<Node *>& children) override;

    std::shared_ptr<SymbolTable> getSymbol(const std::string identifier);
};


#endif //SPLC_SCOPE_H
