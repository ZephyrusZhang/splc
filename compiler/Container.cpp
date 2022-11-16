#include <iostream>
#include <vector>
#include "Container.h"
#include "Specifier.h"
#include "Def.h"

std::shared_ptr<Container> Container::generateContainer(const Node *node) {
    const std::string &tokenName = node->tokenName;
    if (tokenName == "Specifier") {
        return std::make_shared<Specifier>(node);
    } else if (tokenName == "Def") {
        return std::make_shared<Def>(node);
    } else if (tokenName == "VarDec" || tokenName == "Dec") {
        return std::make_shared<Dec>(node);
    }
    return nullptr;
}

const std::string &Container::getTokenName() const noexcept {
    return node->tokenName;
}
