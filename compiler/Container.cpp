#include <iostream>
#include <vector>
#include "Container.h"
#include "Specifier.h"
#include "Def.h"
#include "Scope.h"
#include "Stmt.h"

void Container::generateContainer(Node *node) {
    const std::string &tokenName = node->tokenName;
    if (tokenName == "FunDec" || tokenName == "FOR" || tokenName == "WHILE") {
        // Earlier to generate Scope for FunDec/FOR/WHILE to install FunctionArgs/ForDef into its SymbolTable
        // It's ownership should be transfer to LC when LC trying to create its own Scope.
        auto scope = std::make_shared<Scope>(node, tokenName);
        Scope::globalScopes.push_back(scope);
    }
    if (tokenName == "Specifier") {
        node->container = std::make_shared<Specifier>(node);
    } else if (tokenName == "Def" || tokenName == "ExtDef" || tokenName == "DefOrExp") {
        node->container = std::make_shared<Def>(node);
    } else if (tokenName == "VarDec" || tokenName == "Dec" || tokenName == "FunDec") {
        node->container = std::make_shared<Dec>(node);
    } else if (tokenName == "Stmt") {
        node->container = std::make_shared<Stmt>(node);
    } else if (tokenName == "LC") {
        // generate Scope and bind it to '{' when bison meets LC.
        // FunDec/FOR/WHILE: Take ownership of already generated Scope from FunDec/FOR to myself(LC)
        auto scope = Scope::getCurrentScope();
        if (scope->node && (scope->node->tokenName == "FunDec"
                            || scope->node->tokenName == "FOR"
                            || scope->node->tokenName == "WHILE")) {
            // Transfer ownership of Scope from FunDec/FOR/WHILE to LC
            assert(scope->node->container == nullptr || scope->node->container->type != ContainerType::Scope);
            scope->node->container = nullptr;
            scope->node = node;
            node->container = scope;
        } else {
            node->container = std::make_shared<Scope>(node, tokenName);
            Scope::globalScopes.push_back(node->container->castTo<Scope>());
        }
    } else if (tokenName == "RC") {
        assert(Scope::getCurrentScope()->node->tokenName == "LC");
        Scope::globalScopes.pop_back();
    }
}

const std::string &Container::getTokenName() const noexcept {
    return node->tokenName;
}