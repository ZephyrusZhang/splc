#include "Scope.h"
#include "Node.h"
#include "Dec.h"
#include "CompoundType.h"

std::vector<std::shared_ptr<Scope>> Scope::globalScopes{std::make_shared<Scope>(nullptr, "Global")};

bool Scope::isSymbolExists(const std::string &identifier) const {
    return this->symbols.find(identifier) != this->symbols.end();
}

void Scope::insertSymbol(const std::string &identifier, const std::shared_ptr<Specifier> &specifier,
                         const std::shared_ptr<Dec> &dec) {
    assert(!isSymbolExists(identifier));
    // Set Specifier::isArray
    if (dec)
        symbols[identifier] = std::make_pair(std::make_shared<CompoundType>(*specifier, *dec), SymbolAttribute{});
    else
        symbols[identifier] = std::make_pair(std::make_shared<CompoundType>(*specifier), SymbolAttribute{});
}

bool Scope::isSymbolExistsRecursively(const std::string &identifier) const {
    const Scope *current = this;
    while (current != nullptr && !current->isSymbolExists(identifier))
        current = current->parentScope.get();
    return current != nullptr;
}

Scope::SymbolType Scope::lookupSymbol(const std::string &identifier) {
    Scope *current = this;
    while (current != nullptr && !current->isSymbolExists(identifier))
        current = current->parentScope.get();
    assert(current != nullptr);
    return current->symbols[identifier].first;
}

Scope::Scope(Node *node) : Container(node, containerType) {
    if (globalScopes.empty()) return;
    parentScope = globalScopes.back();
}

Scope::Scope(Node *node, const std::string &generateWithToken) : Scope(node) {
    this->generateWithToken = generateWithToken;
}

void Scope::onThisInstalled() {
    // When LC is reduced into CompSt/StructSpecifier, pop myself from globalScopes;
    if (generateWithToken == "LC" && node != nullptr) {
        if (node->parent->tokenName == "StructSpecifier") {
            // drop owner for Struct->LC, and delete myself
            assert(globalScopes.back().get() == this);
            node->container = nullptr;
            node = nullptr;
            // Release ownership of this Scope.
            // This Scope should be deleted
        }
    }
    globalScopes.pop_back();
}

void Scope::installChild(const std::vector<Node *> &children) {

}

std::shared_ptr<Scope> Scope::getCurrentScope() {
    assert(!globalScopes.empty());
    return globalScopes.back();
}

std::shared_ptr<Scope> Scope::getGlobalScope() {
    assert(!globalScopes.empty());
    return globalScopes[0];
}

void Scope::printSymbolTable() {
    std::cout << "SymbolTable for token " << this->generateWithToken << ":" << std::endl;
    for (const auto &item: this->symbols) {
        std::cout << "\t" << item.first << ": " << *item.second.first;
        if (!item.second.second.empty()) {
            std::cout << "\n\t\tAttrs: ";
            for (const auto &attr: item.second.second) {
                std::cout << "{" << attr.first << ":" << attr.second << "} ";
            }
        }
        std::cout << std::endl;
    }
}

void Scope::setAttribute(const std::string &identifier, const std::string &key, const std::string &value) {
    assert(isSymbolExists(identifier));
    symbols[identifier].second[key] = value;
    printSymbolTable();
}

std::string Scope::getAttribute(const std::string &identifier, const std::string &key) const {
    assert(isSymbolExists(identifier));
    return "";
}
