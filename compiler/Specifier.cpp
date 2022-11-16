#include <cassert>
#include "Specifier.h"
#include "Node.h"
#include "Def.h"

void Specifier::installChild(const std::vector<Node *>& children) {
//    assert(node->tokenName == this->name);
//    for (const auto &item: children)
//        Node::printTree(item, std::cout);
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
                parseStruct(children[0]->children[3]);
        } else throw std::invalid_argument("unexpected specifier, should be pointer");
    } else if (children.size() == 2) {
        assert(children[1]->tokenName == "MUL");
        assert(children[0]->container);
        this->type = TypePointer;
        this->pointTo = children[0]->container->castTo<Specifier>();
    } else throw std::invalid_argument("unexpected specifier");
}

void Specifier::parseStruct(Node *defListRoot) {
//    std::cout << "TODO: parseStruct: " << std::endl;
//    Node::printTree(pNode, std::cout);
//     TODO: Parse Struct Node
}

Specifier::Specifier(Node *node) : Container(node, containerType) {
    type = TypeUnknown;
}

Specifier::Specifier(const Specifier &copy) : Specifier(copy.node) {
    this->type = copy.type;
    this->structName = copy.structName;
    this->structDefList = std::make_unique<std::vector<std::shared_ptr<Def>>>(copy.structDefList.operator*());
    this->pointTo = copy.pointTo;
}

std::ostream& operator<<(std::ostream& os, const Specifier &specifier) {
    os << "{";
    switch (specifier.type) {
        case TypeUnknown: os << "unknown"; break;
        case TypeVoid:  os << "void"; break;
        case TypeChar: os << "char"; break;
        case TypeInt: os << "int"; break;
        case TypeFloat: os << "float"; break;
        case TypeStruct: os << "struct"; break;
        case TypePointer: os << "point to"; break;
    }
    if (specifier.type == TypePointer) os << ": " << *specifier.pointTo;
    else if (specifier.type == TypeStruct) os << ": " << specifier.structName;
    os << "}";
    return os;
}

