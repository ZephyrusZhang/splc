#include "Stmt.h"
#include "Exp.h"
#include "Scope.h"

void Stmt::installChild(const std::vector<Node *> &children) {
    if (this->node->data == "RETURN") {
        if (children.size() == 3) {
            const auto &exp = children[1]->container->castTo<Exp>();
            const auto &funcScope = Scope::getCurrentFunctionScope();
            const auto &funcReturnCompType = Scope::getGlobalScope()->lookupSymbol(funcScope->functionName);
            if (!(exp->getCompoundType() == funcReturnCompType.operator*())) {
                std::cerr << "Error type 8 at line " << this->node->lineno << ": function " << funcScope->functionName
                          << " should return " << *funcReturnCompType << " but given " << exp->getCompoundType()
                          << std::endl;
            }
        }
    }
}

Stmt::Stmt(Node *node) : Container(node, containerType) {}
