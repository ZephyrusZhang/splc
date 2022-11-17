#include "Dec.h"
#include "Scope.h"

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
            int arrSize = std::stoi(children[2]->data);
            if (arrSize <= 0)
                std::cerr << "Error: Array Size " << arrSize << " is invalid at " << node->lineno << std::endl;
            this->arraySize->push_back(arrSize);
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
        this->funcDec = std::make_unique<std::vector<ParmaDec>>();
        if (children.size() == 4) {
            // parse function args and insert into symbol table
            assert(children[2]->tokenName == "VarList");
            assert(Scope::getCurrentScope()->generateWithToken == "FunDec");
            auto parmadecs = Node::convertTreeToVector(children[2], "VarList", {"ParamDec"});
            for (const auto &item: parmadecs) {
                funcDec->emplace_back(item->children[0]->container->castTo<Specifier>(), item->children[1]->container->castTo<Dec>());
            }
            for (const auto &item: parmadecs) {
                assert(item->children.size() == 2);
                assert(item->children[0]->tokenName == "Specifier" && item->children[1]->tokenName == "VarDec");
                auto& id = *item->children[1]->container->castTo<Dec>()->identifier;
                Scope::getCurrentScope()->insertSymbol(id,
                                                       item->children[0]->container->castTo<Specifier>(),
                                                       item->children[1]->container->castTo<Dec>());
                Scope::getCurrentScope()->setAttribute(id, "type", "arg");
            }
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