#include "Scope.h"
#include "Node.h"

void Scope::installChild(std::vector<Node *>) {

}

std::shared_ptr<SymbolTable> Scope::getSymbol(const std::string identifier) {
    return std::shared_ptr<SymbolTable>();
}
