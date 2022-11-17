#include "Node.h"
#include "Exp.h"
#include "Container.h"
#include "Scope.h"
#include "Dec.h"
#include "Def.h"
#include <memory>
#include <vector>

Exp::Exp(Node *node, ExpType expType)
        : Container(node, containerType) {
    this->expType = expType;
}

void Exp::installChild(const std::vector<Node *> &children) {
    if (this->expType == ExpType::IDENTIFIER) {
        const auto &id = children[0]->data;
        if (!Scope::getCurrentScope()->isSymbolExistsRecursively(id)) {
            std::cerr << "Error type 1 at line " << this->node->lineno << ": variable " << id
                      << " is used without definition, guess its type to int" << std::endl;
            this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
        } else {
            this->expCompoundType = Scope::getCurrentScope()->lookupSymbol(id);
        }
    } else if (this->expType == ExpType::FUNC_INVOKE) {
        const auto &id = children[0]->data;
        if (Scope::getGlobalScope()->isSymbolExists(id)) {
            const auto &func = Scope::getGlobalScope()->lookupSymbol(id);
            this->expCompoundType = func; // set return type
            if (children.size() == 4) {
                assert(children[2]->tokenName == "Args");
                // checkArgs
                auto callArgs = Node::convertTreeToVector(children[2], "Args", {"Exp"});
                const auto &defArgs = func->funcArgs.operator*();
                if (!std::equal(defArgs.begin(), defArgs.end(), callArgs.begin(), callArgs.end(),
                                [](const CompoundType &defArg, const Node *arg) {
                                    return defArg == *arg->container->castTo<Exp>()->expCompoundType;
                                })) {
                    std::cerr << "Error type 9 at line " << this->node->lineno << ": function " << id
                              << "is called with mismatched args" << std::endl;
                }
            }
        } else {
            std::cerr << "Error type 2 at line " << this->node->lineno << ": function " << id
                      << " is invoked without definition" << std::endl;
        }
    } else if (this->expType == ExpType::LITERAL_INT) {
        this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
    } else if (this->expType == ExpType::PLUS) {
        auto left = children[0]->container->castTo<Exp>();
        auto right = children[2]->container->castTo<Exp>();
//        if (!left->expCompoundType)
    }
}
