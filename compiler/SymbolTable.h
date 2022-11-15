#ifndef SPLC_SYMBOLTABLE_H
#define SPLC_SYMBOLTABLE_H
#include <string>
#include "Specifier.h"

enum SymbolType {
    Varible = 0,
    Funtion
};

class SymbolTable {
public:
    const SymbolType symbolType;
    const std::string identifier;
    // For varible
    const std::shared_ptr<Specifier> specifier;
    // For function
    const std::vector<SymbolTable> functionArgs;

    explicit SymbolTable(SymbolType symbolType);
};


#endif //SPLC_SYMBOLTABLE_H
