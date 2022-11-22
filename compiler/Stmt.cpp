#include "Stmt.h"
#include "Exp.h"
#include "Scope.h"

Stmt::Stmt(Node *node, StmtType stmtType) : Container(node, containerType) {
    this->stmtType = stmtType;
}

void Stmt::installChild(const std::vector<Node *> &children) {
    if (stmtType == StmtType::FOR || stmtType == StmtType::WHILE) {
        Node *forOrWhile = children[0];
        // RC should be met already and current Scope shouldn't be bound to forOrWhile node
        auto scope = Scope::getCurrentScope();
        if (scope->node == forOrWhile) {
            std::cout << "pop orphan Scope for " << forOrWhile->tokenName << " at line " << forOrWhile->lineno
                      << std::endl;
            Scope::globalScopes.pop_back();
            assert(scope->node->container == nullptr);
            scope->node->container = scope;
        }
    }
    int conditionalExpIdx = -1;
    switch (stmtType) {
        case StmtType::IF:
            conditionalExpIdx = 2;
            break;
        case StmtType::IF_ELSE:
            conditionalExpIdx = 2;
            break;
        case StmtType::WHILE:
            conditionalExpIdx = 2;
            break;
        case StmtType::FOR:
            conditionalExpIdx = 4;
            break;
        default:
            conditionalExpIdx = -1;
            break;
    }
    if (conditionalExpIdx != -1) {
        const auto &conditionalExp = children[conditionalExpIdx]->container->castTo<Exp>();
        if (!(conditionalExp->expType == ExpType::AND || conditionalExp->expType == ExpType::OR ||
              conditionalExp->expType == ExpType::LT || conditionalExp->expType == ExpType::LE ||
              conditionalExp->expType == ExpType::GT || conditionalExp->expType == ExpType::GE ||
              conditionalExp->expType == ExpType::EQ || conditionalExp->expType == ExpType::NE ||
              conditionalExp->getCompoundType().type == TypeInt)) {
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

