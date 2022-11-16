#ifndef SPLC_SCOPE_H
#define SPLC_SCOPE_H

#include <vector>
#include <map>
#include "Node.h"
#include "Container.h"
#include "Specifier.h"

class Dec;
class Scope : public Container {
private:
    typedef std::pair<std::string, std::string> SymbolAttribute;
    typedef std::pair<std::shared_ptr<Specifier>, std::shared_ptr<Dec>> SymbolType;
    std::map<std::string, std::pair<SymbolType, std::vector<SymbolAttribute>>> symbols;
public:
    const static ContainerType containerType = ContainerType::Scope;
    static std::vector<std::shared_ptr<Scope>> globalScopes;

    std::shared_ptr<Scope> parentScope;

    explicit Scope(Node * node);
    ~Scope() override {
        std::cout << "delete Scope!" << std::endl;
    };
    void installChild(const std::vector<Node *>& children) override;
    void onThisInstalled() override;

    bool isSymbolExists(const std::string& identifier) const;
    void insertSymbol(const std::string &identifier, const std::shared_ptr<Specifier>& specifier, const std::shared_ptr<Dec>& dec);
    std::pair<std::shared_ptr<Specifier>, std::shared_ptr<Dec>> lookupSymbol(const std::string &identifier);
    void setAttribute(const std::string &identifier, const std::string &key, const std::string value);
    std::string getAttribute(const std::string &identifier, const std::string &key) const;

    static std::shared_ptr<Scope> getCurrentScope();

    void printSymbolTable();
};


#endif //SPLC_SCOPE_H
