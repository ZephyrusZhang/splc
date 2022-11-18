#include "Def.h"
#include "Scope.h"
#include "Exp.h"

void Def::installChild(const std::vector<Node *> &children) {
    if (getTokenName() == "Def") {
        assert(children.size() == 3);
        assert(children[0]->tokenName == "Specifier");
        assert(children[1]->tokenName == "DecList");
        assert(children[2]->tokenName == "SEMI");
        this->specifier = children[0]->container->castTo<Specifier>();
        parseDecList();
        // install variable definition into symbolTable
        for (const auto &item: this->declares) {
            if (!Scope::getCurrentScope()->isSymbolExists(*item->identifier)) {
                Scope::getCurrentScope()->insertSymbol(*item->identifier, this->specifier,
                                                       item->node->container->castTo<Dec>());
                Scope::getCurrentScope()->setAttribute(*item->identifier, "type", "variable");
                // Type Checking for DecList
                if (item->node->container->castTo<Dec>()->hasInitialValue) {
                    CompoundType left(*this->specifier, *item->node->container->castTo<Dec>());
                    const auto &right = item->node->children[2]->container->castTo<Exp>()->getCompoundType();
                    if (!CompoundType::canAssignment(left, right)) {
                        std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                                  << left
                                  << " and " << right << " in the assignment." << std::endl;
                    }
                }
            } else {
                std::cerr << "Error type 3 at line " << this->node->lineno << ": variable " << *item->identifier << " is already defined" << std::endl;
            }
        }
    } else if (getTokenName() == "ExtDef") {
        // ExtDef
        assert(children[0]->tokenName == "Specifier");
        this->specifier = children[0]->container->castTo<Specifier>();
        if (children[1]->tokenName == "ExtDecList") {
            parseExtDecList();
            // insert global variable definition into symbolTable
            for (const auto &item: this->declares) {
                const auto id = *item->identifier;
                if (!Scope::getCurrentScope()->isSymbolExists(*item->identifier)) {
                    Scope::getCurrentScope()->insertSymbol(*item->identifier, this->specifier,
                                                           item->node->container->castTo<Dec>());
                    Scope::getCurrentScope()->setAttribute(*item->identifier, "type", "global variable");
                } else {
                    std::cerr << "Error type 3 at line " << this->node->lineno << ": variable " << *item->identifier << " is already defined" << std::endl;
                }
            }
        } else if (children[1]->tokenName == "SEMI") {
            if (this->specifier->type == TypeStruct) {
                // insert struct definition.
                if (!Scope::getCurrentScope()->isSymbolExists(this->specifier->structName)) {
                    Scope::getCurrentScope()->insertSymbol(this->specifier->structName, this->specifier, nullptr);
                    Scope::getCurrentScope()->setAttribute(this->specifier->structName, "type", "struct");
                } else {
                    std::cerr << "Error type 15 at line " << this->node->lineno << ": structure " << this->specifier->structName << " is already defined" << std::endl;
                }
            } else {
                std::cerr << "Warning: useless definition at line " << this->node->lineno << " " << *this->specifier
                          << std::endl;
            }
        } else if (children[1]->tokenName == "FunDec") {
            // insert function definition.
//            const auto &funcDec = children[1]->container->castTo<Dec>();
//            const auto &identifier = *funcDec->identifier;
//            if (!Scope::getGlobalScope()->isSymbolExists(identifier)) {
//                Scope::getGlobalScope()->insertSymbol(identifier, this->specifier, funcDec);
//                Scope::getGlobalScope()->setAttribute(identifier, "type", "function");
//            } else {
//                std::cerr << "Error type 4 at line " << this->node->lineno << ": function " << identifier << " is already defined" << std::endl;
//            }
        }
    } else if (getTokenName() == "DefOrExp") {
        if (children.size() == 2 && children[1]->tokenName == "DecList") {
            this->specifier = children[0]->container->castTo<Specifier>();
            parseDecList();
            // install variable declaration in FOR into symbolTable
            for (const auto &item: this->declares) {
                if (!Scope::getCurrentScope()->isSymbolExists(*item->identifier)) {
                    Scope::getCurrentScope()->insertSymbol(*item->identifier, this->specifier,
                                                           item->node->container->castTo<Dec>());
                    Scope::getCurrentScope()->setAttribute(*item->identifier, "type", "for variable");
                    // Type Checking for DecList
                    if (item->node->container->castTo<Dec>()->hasInitialValue) {
                        CompoundType left(*this->specifier, *item->node->container->castTo<Dec>());
                        const auto &right = item->node->children[2]->container->castTo<Exp>()->getCompoundType();
                        if (!CompoundType::canAssignment(left, right)) {
                            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                                      << left
                                      << " and " << right << " in the assignment." << std::endl;
                        }
                    }
                } else {
                    std::cerr << "Error type 3 at line " << this->node->lineno << ": variable " << *item->identifier
                              << " is already defined" << std::endl;
                }
            }
        }
    } else throw std::runtime_error("Def: bad TokenName " + getTokenName());
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