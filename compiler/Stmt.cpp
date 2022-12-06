#include "Stmt.h"
#include "Exp.h"
#include "Scope.h"

Stmt::Stmt(Node *node, StmtType stmtType) : Container(node, containerType) {
    this->stmtType = stmtType;
}

void Stmt::installChild(const std::vector<Node *> &children) {
    if (stmtType == StmtType::FOR || stmtType == StmtType::WHILE) {
        Node *forOrWhile = children[0];
        // If CompSt is present, RC should be met already, thus currentScope should be bound to LC, and current Scope shouldn't be bound to forOrWhile node
        auto scope = Scope::getCurrentScope();
        if (scope->node == forOrWhile) {
            std::cout << "transfer orphan Scope to " << forOrWhile->tokenName << " at line " << forOrWhile->lineno
                      << std::endl;
            Scope::globalScopes.pop_back();
            assert(scope->node->container == nullptr);
            scope->node->container = scope;
            assert(forOrWhile->container == scope);
        }
    }

    if (stmtType == StmtType::IF || stmtType == StmtType::IF_ELSE ||
        stmtType == StmtType::WHILE || stmtType == StmtType::FOR) {
        int conditionalExpIdx = (stmtType == StmtType::FOR) ? 4 : 2;
        const auto &conditionalExp = children[conditionalExpIdx]->container->castTo<Exp>();
        if (!((1 <= (int) conditionalExp->expType && (int) conditionalExp->expType <= 8) || conditionalExp->getCompoundType().type == TypeInt || conditionalExp->getCompoundType().type == TypePointer)) {
            std::cerr << "Error at line " << this->node->lineno << ": "
                      << "non-boolean expression at the conditional statement." << std::endl;
        }
    } else if (stmtType == StmtType::BREAK || stmtType == StmtType::CONTINUE) {
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
    } else if (stmtType == StmtType::RETURN) {
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

