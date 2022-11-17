#include "Scope.h"
#include "Node.h"
#include "Dec.h"

std::vector<std::shared_ptr<Scope>> Scope::globalScopes{std::make_shared<Scope>(nullptr)};

void Scope::installChild(const std::vector<Node *> &children) {

}

bool Scope::isSymbolExists(const std::string &identifier) const {
    return this->symbols.find(identifier) != this->symbols.end();
}

void Scope::insertSymbol(const std::string &identifier, const std::shared_ptr<Specifier> &specifier,
                         const std::shared_ptr<Dec> &dec) {
    assert(!isSymbolExists(identifier));
    // Set Specifier::isArray
    specifier->isArray = dec->isArray();
    symbols[identifier] = std::make_pair(std::make_pair(specifier, dec), std::vector<SymbolAttribute>{});
    printSymbolTable();
}

bool Scope::isSymbolExistsRecursively(const std::string &identifier) const {
    const Scope* current = this;
    while(current != nullptr && !current->isSymbolExists(identifier))
        current = current->parentScope.get();
    return current != nullptr;
}

std::pair<std::shared_ptr<Specifier>, std::shared_ptr<Dec>> Scope::lookupSymbol(const std::string &identifier) {
    Scope* current = this;
    while(current != nullptr && !current->isSymbolExists(identifier))
        current = current->parentScope.get();
    assert(current != nullptr);
    return current->symbols[identifier].first;
}

Scope::Scope(Node *node) : Container(node, containerType) {
    if (globalScopes.empty()) return;
    parentScope = globalScopes.back();
}

void Scope::onThisInstalled() {
    // check owner and transfer owner
    if (node->parent->tokenName == "StructSpecifier") {
        // drop owner and assert this scope is not changed
        assert(globalScopes.back().get() == this);
        globalScopes.pop_back();
        node->container = nullptr;
    } else if (node->parent->tokenName == "CompSt") {
        // transfer owner to CompSt
        std::cout << "Transfer Scope ownership to CompSt " << node->tokenName << std::endl;
        assert(globalScopes.back().get() == this);
        globalScopes.pop_back();
        node->parent->container = std::move(node->container);
        this->node = node->parent;
    }
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
    std::cout << "SymbolTable: " << std::endl;
    for (const auto &item: this->symbols) {
        std::cout << "\t" << item.first << ": " << *(item.second.first.first);
        if (item.second.first.second)
            std::cout << ", " << *(item.second.first.second);
        std::cout << std::endl;
    }
}

void Scope::setAttribute(const std::string &identifier, const std::string &key, const std::string &value) {
    assert(isSymbolExists(identifier));
    symbols[identifier].second.emplace_back(key, value);
}

std::string Scope::getAttribute(const std::string &identifier, const std::string &key) const {
    assert(isSymbolExists(identifier));
    return "";
}

std::ostream &operator<<(std::ostream &os, Scope::SymbolType &symbolType) {
    os << "{" << *symbolType.first;
    if (symbolType.second) os << "," << *symbolType.second;
    os << "}";
    return os;
}