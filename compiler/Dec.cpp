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
    } else throw std::runtime_error("bad Type in Dec: " + this->getTokenName());
}

bool Dec::isArray() const noexcept {
    return arraySize && !arraySize->empty();
}

void Dec::installToSymbolTable(std::shared_ptr<Specifier> &specifier) {

}

std::ostream &operator<<(std::ostream &os, const Dec &dec) {
    os << "Dec{id:" << *dec.identifier;
    if (dec.arraySize) {
        os << " ,arr";
        for (const auto &item: *dec.arraySize)
            os << "[" << item << "]";
    }
    os << "}";
    return os;
}