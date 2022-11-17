#include "Dec.h"

void Dec::installChild(const std::vector<Node *> &children) {
    if (this->getTokenName() == "VarDec") {
        if (children.size() == 1 && children[0]->tokenName == "ID") {
            this->identifier = std::make_unique<std::string>(children[0]->data);
            // copy and generate a unique_ptr
        } else if (children.size() == 4 && children[0]->tokenName == "VarDec") {
            const auto parent = children[0]->container->castTo<Dec>();
            // parent Dec instance
            this->identifier = std::move(parent->identifier);
            if (parent->arraySize) this->arraySize = std::move(parent->arraySize);
            else this->arraySize = std::make_unique<std::vector<int>>();
            this->arraySize->push_back(std::stoi(children[2]->data));
        }
    } else if (this->getTokenName() == "Dec") {
        assert(children[0]->tokenName == "VarDec");
        const auto parent = children[0]->container->castTo<Dec>();
        // parent Dec instance
        this->identifier = std::move(parent->identifier);
        this->arraySize = std::move(parent->arraySize);
        if (children.size() == 3) this->initialValue = std::unique_ptr<Node>(children[2]);
        // contains initialValue
    } else if (this->getTokenName() == "FunDec") {
        this->identifier = std::make_unique<const std::string>(children[0]->data);
        this->funcDec = std::make_unique<std::vector<Dec::ParmaDec>>();
        if (children.size() == 4) {
            assert(children[2]->tokenName == "VarList");
            auto parmadecs = Node::convertTreeToVector(children[2], "VarList", {"ParamDec"});
            for (const auto &item: parmadecs) {
                assert(item->children.size() == 2);
                assert(item->children[0]->tokenName == "Specifier" && item->children[1]->tokenName == "VarDec");
                this->funcDec->emplace_back(item->children[0]->container->castTo<Specifier>(),
                                            item->children[1]->container->castTo<Dec>());
            }
        }
        // install FunDec to SymbolTable
        std::cout << "FunDec: id:" << *this->identifier << std::endl;
        for (const auto &item: *this->funcDec) {
            std::cout << "\t" << *item.first << ": " << *item.second << std::endl;
        }
    } else throw std::runtime_error("bad Type in Dec: " + this->getTokenName());
}

bool Dec::isArray() const noexcept {
    return arraySize && !arraySize->empty();
}

std::ostream &operator<<(std::ostream &os, const Dec &dec) {
    if (!dec.identifier) {
        return os;
    }
    os << "Dec{id:" << *dec.identifier;
    if (dec.arraySize) {
        os << " ,arr";
        for (const auto &item: *dec.arraySize)
            os << "[" << item << "]";
    }
    os << "}";
    return os;
}

bool Dec::isFunction() const noexcept {
    return !this->funcDec->empty();
}
