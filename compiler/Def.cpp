#include "Def.h"
#include "Scope.h"

void Def::installChild(const std::vector<Node *> &children) {
    if (getTokenName() == "Def") {
        assert(children.size() == 3);
        assert(children[0]->tokenName == "Specifier");
        assert(children[1]->tokenName == "DecList");
        assert(children[2]->tokenName == "SEMI");
        this->specifier = children[0]->container->castTo<Specifier>();
        parseDecList();
        // install decs into symbolTable
        for (const auto &item: this->declares) {
            if (checkAlreadyDefined(*item->identifier))
                Scope::getCurrentScope()->insertSymbol(*item->identifier, this->specifier,
                                                       item->node->container->castTo<Dec>());
            Scope::getCurrentScope()->setAttribute(*item->identifier, "type", "variable");
        }
    } else if (getTokenName() == "ExtDef") {
        // ExtDef
        assert(children[0]->tokenName == "Specifier");
        this->specifier = children[0]->container->castTo<Specifier>();
        if (children[1]->tokenName == "ExtDecList") {
            parseExtDecList();
            // install extDecs into symbolTable
            for (const auto &item: this->declares) {
                const auto id = *item->identifier;
                if (checkAlreadyDefined(*item->identifier))
                    Scope::getCurrentScope()
                    ->insertSymbol(*item->identifier, this->specifier, item->node->container->castTo<Dec>());
                Scope::getCurrentScope()->setAttribute(*item->identifier, "type", "variable");
            }
        } else if (children[1]->tokenName == "SEMI") {
            if (this->specifier->type == TypeStruct) {
                if (checkAlreadyDefined(this->specifier->structName)) {
                    Scope::getCurrentScope()->insertSymbol(this->specifier->structName, this->specifier, nullptr);
                    Scope::getCurrentScope()->setAttribute(this->specifier->structName, "type", "struct");
                } else {
                    std::cerr << "Error: struct " << this->specifier->structName << " already defined." << std::endl;
                }
            } else {
                std::cerr << "Warning: useless definition at line " << this->node->lineno << " " << *this->specifier << std::endl;
            }
        } else if (children[1]->tokenName == "FunDec") {

        }
    }
}

Def::Def(Node *node) : Container(node, containerType) {

}

void Def::parseDecList() {
    const Node *decListRoot = this->node->children[1];
    const auto decNodes = Node::convertTreeToVector(decListRoot, "DecList", {"Dec"});
    for (const auto &item: decNodes) {
        this->declares.push_back(item->container->castTo<Dec>());
    }
}

void Def::parseExtDecList() {
    const Node *extDecListRoot = this->node->children[1];
    const auto extDecNodes = Node::convertTreeToVector(extDecListRoot, "ExtDecList", {"VarDec"});
    for (const auto &item: extDecNodes) {
        this->declares.push_back(item->container->castTo<Dec>());
    }
}

bool Def::checkAlreadyDefined(const std::string &identifier) {
    if (Scope::getCurrentScope()->isSymbolExists(identifier)) {
        auto predefined = Scope::getCurrentScope()->lookupSymbol(identifier);
        std::cerr << "Error: identifier " << identifier
                  << " is already defined at line " << predefined.first->node->lineno
                  << ": " << predefined << std::endl;
        return false;
    }
    return true;
}
