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
        if (tokenName == "FunDec") {
            // set functionName for generated Scope, we use FunDec -> ID LP VarList RP | ID LP RP to get the Node* of ID.
            extern Node **yystack;
            Node * identifier = yystack[-2];
            if (identifier->tokenName == "LP") identifier = yystack[-3];
            assert(identifier->tokenName == "ID");
            scope->functionName = identifier->data;
        }
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
            scope->node->container = nullptr;
            scope->node = node;
            node->container = scope;
        } else {
            node->container = std::make_shared<Scope>(node, tokenName);
            Scope::globalScopes.push_back(node->container->castTo<Scope>());
        }
    } else if (tokenName == "RC") {
        Scope::globalScopes.pop_back();
    }
}

const std::string &Container::getTokenName() const noexcept {
    return node->tokenName;
}