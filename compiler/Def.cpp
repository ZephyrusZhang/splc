#include "Def.h"
#include "Scope.h"

void Def::installChild(const std::vector<Node *>& children)  {
    assert(children.size() == 3);
    assert(children[0]->tokenName == "Specifier");
    assert(children[1]->tokenName == "DecList");
    assert(children[2]->tokenName == "SEMI");
    this->specifier = children[0]->container->castTo<Specifier>();
    parseDecList();
    // install decs into symbolTable
    for (const auto &item: this->declares) {
        Scope::getCurrentScope()->insertSymbol(*item->identifier, this->specifier, item->node->container->castTo<Dec>());
    }
}

Def::Def(Node *node) : Container(node, containerType) {

}

void Def::parseDecList() {
    const Node * decListRoot = this->node->children[1];
    const auto decNodes = Node::convertTreeToVector(decListRoot, "DecList", {"Dec"});
    for (const auto &item: decNodes) {
        this->declares.push_back(item->container->castTo<Dec>());
    }
}
