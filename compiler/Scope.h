#ifndef SPLC_SCOPE_H
#define SPLC_SCOPE_H

#include <vector>
#include <map>
#include "Node.h"
#include "Container.h"
#include "Specifier.h"
#include "CompoundType.h"

class Dec;
class Scope : public Container {
public:
    typedef std::map<std::string, std::string> SymbolAttribute;
    typedef std::shared_ptr<CompoundType> SymbolType;
private:
    std::map<std::string, std::pair<SymbolType, SymbolAttribute>> symbols;
public:
    static std::vector<std::shared_ptr<Scope>> globalScopes;

    const std::string generateWithToken;
    std::shared_ptr<Scope> parentScope;
    std::string functionName;

    explicit Scope(Node * node, std::string generateWithToken);
    ~Scope() override {
        std::cout << "delete Scope!" << std::endl;
    };
    void installChild(const std::vector<Node *>& children) override;
    void onThisInstalled() override;

    bool isSymbolExists(const std::string& identifier) const;
    bool isSymbolExistsRecursively(const std::string& identifier) const;
    void insertSymbol(const std::string &identifier, const std::shared_ptr<Specifier>& specifier, const std::shared_ptr<Dec>& dec);
    SymbolType lookupSymbol(const std::string &identifier);
    void setAttribute(const std::string &identifier, const std::string &key, const std::string& value);
    std::string getAttribute(const std::string &identifier, const std::string &key) const;

    static std::shared_ptr<Scope> getCurrentScope();
    static std::shared_ptr<Scope> getCurrentFunctionScope();
    static std::shared_ptr<Scope> getGlobalScope();

    void printSymbolTable();
};
std::ostream& operator<<(std::ostream& os, Scope::SymbolType& symbolType);

#endif //SPLC_SCOPE_H
