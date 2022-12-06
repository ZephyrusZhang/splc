#include "Scope.h"

#include <utility>
#include "Node.h"
#include "Dec.h"
#include "ir/IR.h"
#include "CompoundType.h"

std::vector<std::shared_ptr<Scope>> Scope::globalScopes{std::make_shared<Scope>(nullptr, "Global")};

bool Scope::isSymbolExists(const std::string &identifier) const {
    return this->symbols.find(identifier) != this->symbols.end();
}

void Scope::insertSymbol(const std::string &identifier, const std::shared_ptr<Specifier> &specifier,
                         const std::shared_ptr<Dec> &dec) {
    assert(!isSymbolExists(identifier));
    assert(specifier->type != TypeUnknown);
    // Set Specifier::isArray
    if (dec)
        symbols[identifier] = std::make_pair(std::make_shared<CompoundType>(*specifier, *dec), SymbolAttribute{});
    else
        symbols[identifier] = std::make_pair(std::make_shared<CompoundType>(*specifier), SymbolAttribute{});
    if (!unresolvedStructs.empty()) {
        auto it = unresolvedStructs.begin();
        while(it != unresolvedStructs.end()) {
            const auto item = *it;
            if (isSymbolExists(*item->unresolvedStructName)) {
                const auto sym = symbols[*item->unresolvedStructName];
                assert(sym.first->type == TypeStruct);
                item->structDefLists = symbols[*item->unresolvedStructName].first->structDefLists;
                unresolvedStructs.erase(it);
            } else {
                item->structDefLists = std::make_shared<std::vector<CompoundType::StructDefList>>();
                it++;
            }
        }
    }
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

Scope::Scope(Node *node, std::string generateWithToken)
        : Container(node, containerType), generateWithToken(std::move(generateWithToken)) {
    if (!globalScopes.empty())
        parentScope = globalScopes.back();
}

void Scope::onThisInstalled() {

}

void Scope::installChild(const std::vector<Node *> &children) {

}

std::shared_ptr<Scope> Scope::getCurrentScope() {
    assert(!globalScopes.empty());
    return globalScopes.back();
}

std::shared_ptr<Scope> Scope::getCurrentFunctionScope() {
    for (auto it = Scope::globalScopes.rbegin(); it != Scope::globalScopes.rend(); ++it) {
        if (!it->get()->functionName.empty()) return *it;
    }
    throw std::runtime_error("can't get current scope");
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
    // printSymbolTable();
}

std::string Scope::getAttribute(const std::string &identifier, const std::string &key) const {
    assert(isSymbolExists(identifier));
    return "";
}
