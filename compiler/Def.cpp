#include "Def.h"

void Def::installChild(std::vector<Node *> vector) {
    assert(vector.size() == 3);
    assert(vector[0]->tokenName == "Specifier");
    assert(vector[1]->tokenName == "DecList");
    assert(vector[2]->tokenName == "SEMI");
    this->specifier = vector[0]->container->castTo<Specifier>();
    parseDecList();
    for (const auto &item: declares)
    std::cout << "dec: id:" << *item->identifier << std::endl;
}

Def::Def(const Node *node) : Container(node, containerType) {

}

void Def::parseDecList() {
    const Node * decListRoot = this->node->children[1];
    const auto decNodes = Node::convertTreeToVector(decListRoot, "DecList", {"Dec"});
    for (const auto &item: decNodes) {
        this->declares.push_back(item->container->castTo<Dec>());
    }
}
