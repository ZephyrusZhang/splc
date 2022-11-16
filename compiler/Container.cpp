#include <iostream>
#include <vector>
#include "Container.h"
#include "Specifier.h"
#include "Def.h"
#include "Scope.h"

std::shared_ptr<Container> Container::generateContainer(Node *node) {
    const std::string &tokenName = node->tokenName;
    if (tokenName == "Specifier") {
        node->container = std::make_shared<Specifier>(node);
    } else if (tokenName == "Def") {
        node->container = std::make_shared<Def>(node);
    } else if (tokenName == "VarDec" || tokenName == "Dec") {
        node->container = std::make_shared<Dec>(node);
    } else if (tokenName == "LC") {
        // generate Scope and bind it to '{' when bison meets LC.
        node->container = std::make_shared<Scope>(node);
        Scope::globalScopes.push_back(node->container->castTo<Scope>());
    }
    return nullptr;
}

const std::string &Container::getTokenName() const noexcept {
    return node->tokenName;
}
