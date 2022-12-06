#include <sstream>
#include "IR.h"
#include "Scope.h"

void IR::insertComment(std::ostream &ostream) const {
    if (!comment.empty()) ostream << " ; " << comment;
}

void LabelDefIR::generateIr(std::ostream &ostream) {
    ostream << "LABEL " << this->label << " :";
    insertComment(ostream);
    ostream << std::endl;
}


FunctionDef::FunctionDef(std::string& identifier) : IR(IRType::FunctionDef) {
    assert(Scope::getGlobalScope()->isSymbolExists(identifier));
    this->functionName = identifier;
    this->functionType = Scope::getGlobalScope()->lookupSymbol(identifier);
    std::ostringstream ss;
    ss << "function " << identifier << *this->functionType << std::endl;
    this->comment = ss.str();
}

void FunctionDef::generateIr(std::ostream &ostream) {
    ostream << "FUNCTION " << this->functionName << " :";
    insertComment(ostream);
    ostream << std::endl;
    // generate Params
    for (const auto &item: this->functionType->funcArgs.operator*()) {
        ostream << "PARAM " << item.first << " ; Type:" << item.second << std::endl;
    }
}

