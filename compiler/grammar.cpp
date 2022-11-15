#include <iostream>
#include <vector>
#include "grammar.h"
#include "type.h"

std::shared_ptr<Container> Container::generateContainer(const Node *node) {
    const std::string &tokenName = node->tokenName;
    if (tokenName == "Specifier") {
        std::cout << "meet Specifier " << tokenName << std::endl;
        return std::make_shared<Specifier>(node);
    }
    return nullptr;
}

const std::string &Container::getTokenName() {
    return node->tokenName;
}

void Scope::installChild(std::vector<Node *>) {

}
