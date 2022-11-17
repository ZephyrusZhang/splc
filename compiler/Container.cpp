#include <iostream>
#include <vector>
#include "Container.h"
#include "Specifier.h"
#include "Def.h"
#include "Scope.h"

void Container::generateContainer(Node *node) {
    const std::string &tokenName = node->tokenName;
    if (tokenName == "FunDec" || tokenName == "FOR") {
        // generate Scope for FunDec/For to install FunctionArgs/ForDef into its SymbolTable
        // It's ownership should be transfer to LC when LC trying to create its own Scope.
        auto scope = std::make_shared<Scope>(nullptr, tokenName);
        Scope::globalScopes.push_back(scope);
        // The Scope is "orphan" because its node is nullptr, but later the LC will take it.
    }
    if (tokenName == "Specifier") {
        node->container = std::make_shared<Specifier>(node);
    } else if (tokenName == "Def" || tokenName == "ExtDef" || tokenName == "DefOrExp") {
        node->container = std::make_shared<Def>(node);
    } else if (tokenName == "VarDec" || tokenName == "Dec" || tokenName == "FunDec") {
        node->container = std::make_shared<Dec>(node);
    } else if (tokenName == "LC") {
        // generate Scope and bind it to '{' when bison meets LC.
        // FunDec/FOR: Take ownership of already generated Scope from FunDec/FOR to myself(LC)
        if (Scope::getCurrentScope()->generateWithToken == "FunDec" || Scope::getCurrentScope()->generateWithToken == "FOR") {
            // Don't generate Scope if another Scope is already generated in FOR/FunDec.
            auto scope = Scope::getCurrentScope();
            scope->node = node;
            node->container = scope;
            scope->generateWithToken = "LC";
            return;
        }
        node->container = std::make_shared<Scope>(node, tokenName);
        Scope::globalScopes.push_back(node->container->castTo<Scope>());
    }
}

const std::string &Container::getTokenName() const noexcept {
    return node->tokenName;
}