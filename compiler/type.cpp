#include <cassert>
#include "type.h"
#include "node.h"

void Specifier::installChild(std::vector<Node *> children) {
//    assert(node->tokenName == this->name);
    for (const auto &item: children)
        Node::printTree(item, std::cout);
    if (children.size() == 1) {
        if (children[0]->tokenName == "TYPE") {
            const auto &t = children[0]->data;
            if (t == "int") this->type = TypeInt;
            else if (t == "void") this->type = TypeVoid;
            else if (t == "char") this->type = TypeChar;
            else if (t == "float") this->type = TypeFloat;
            else throw std::invalid_argument("unexpected type: " + t);
        } else if (children[0]->tokenName == "StructSpecifier") {
            assert(children[0]->children[0]->tokenName == "STRUCT");
            assert(children[0]->children[1]->tokenName == "ID");
            this->structName = children[0]->children[1]->data;
            this->type = TypeStruct;
            if (children[0]->children.size() > 2)
                parseStruct(children[0]->children[2]);
        } else throw std::invalid_argument("unexpected specifier, should be pointer");
    } else if (children.size() == 2) {
        assert(children[1]->tokenName == "MUL");
        assert(children[0]->info);
        this->type = TypePointer;
        this->pointTo = children[0]->info->castTo<Specifier>();
    } else throw std::invalid_argument("unexpected specifier");
}

void Specifier::parseStruct(Node *pNode) {
    std::cout << "TODO: parseStruct: " << std::endl;
    Node::printTree(pNode, std::cout);
    // TODO: Parse Struct Node
}
