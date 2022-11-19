#include "Stmt.h"
#include "Exp.h"
#include "Scope.h"

void Stmt::installChild(const std::vector<Node *> &children) {
    if (this->node->data == "BREAK" || this->node->data == "CONTINUE") {
        auto curScope = Scope::getCurrentScope();
        bool isWithinLoop = false;
        while (curScope != Scope::getGlobalScope()) {
            if (curScope->generateWithToken == "WHILE" || curScope->generateWithToken == "FOR") {
                isWithinLoop = true;
                break;
            }
            curScope = curScope->parentScope;
        }
        if (!isWithinLoop) {
            std::cerr << "Error at line " << this->node->lineno << ": "
                      << "break or continue statement not within loop." << std::endl;
        }
    }
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

bool Stmt::isWhileStmt() const {
    return this->node->children[0]->tokenName == "WHILE";
}

bool Stmt::isForStmt() const {
    return this->node->children[0]->tokenName == "FOR";
}

