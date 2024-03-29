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
        if (children.size() == 3) {
            // need Type Checking for ASSIGN, defer it to be done when Def->Dec and install Symbol
            this->hasInitialValue = true;
            this->initialValueExpNode = children[2];
        }
        // contains initialValue
    } else if (this->getTokenName() == "FunDec") {
        this->identifier = std::make_unique<const std::string>(children[0]->data);
        this->funcDec = std::make_unique<std::vector<ParmaDec>>();
        // set functionName for current function Scope
        assert(Scope::getCurrentScope()->node == this->node);
        Scope::getCurrentScope()->functionName = children[0]->data;
        extern Node **yystack;
        Node *specifierNode = yystack[-children.size()];
        assert(specifierNode->tokenName == "Specifier");
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
                auto &id = *item->children[1]->container->castTo<Dec>()->identifier;
                Scope::getCurrentScope()->insertSymbol(id,
                                                       item->children[0]->container->castTo<Specifier>(),
                                                       item->children[1]->container->castTo<Dec>());
                Scope::getCurrentScope()->setAttribute(id, "type", "arg");
            }
        }
        // insert function definition even when CompSt is not reduced in grammar "ExtDef -> Specifier FunDec CompSt".
        // we stole Specifier from bison's stack yyvsp.
        if (!Scope::getGlobalScope()->isSymbolExists(*identifier)) {
            Scope::getGlobalScope()->insertSymbol(*identifier, specifierNode->container->castTo<Specifier>(), castTo<Dec>());
            Scope::getGlobalScope()->setAttribute(*identifier, "type", "function");
        } else {
            std::cerr << "Error type 4 at line " << this->node->lineno << ": function " << *identifier
                      << " is already defined" << std::endl;
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