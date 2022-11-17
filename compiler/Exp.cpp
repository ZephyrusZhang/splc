#include "Node.h"
#include "Exp.h"
#include "Container.h"
#include "Scope.h"
#include <memory>
#include <vector>

Exp::Exp(Node *node, ExpType expType)
        : Container(node, containerType) {
    this->expType = expType;
}
void Exp::installChild(const std::vector<Node *> &children) {
    if (this->expType == ExpType::IDENTIFIER) {
        const auto& id = children[0]->data;
        if (!Scope::getCurrentScope()->isSymbolExistsRecursively(id)) {
            std::cerr << "Error type 1 at line " << this->node->lineno << ": variable " << id << " is used without definition, guess its type to int" << std::endl;
            this->expSpecifier = std::make_shared<Specifier>(this->node);
            this->expSpecifier->type == TypeInt;
        } else {
            this->expSpecifier = Scope::getCurrentScope()->lookupSymbol(id).first;
        }
    } else if (this->expType == ExpType::PLUS) {
        auto left = children[0]->container->castTo<Exp>();
        auto right = children[2]->container->castTo<Exp>();
    } else if (expType == ExpType::FUNC_INVOKE) {
        const auto& funcName = children[0]->data;
    }
}