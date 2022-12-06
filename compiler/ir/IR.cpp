#include <sstream>
#include "IR.h"
#include "../Scope.h"

IRVariable::IRVariable(const IRVariableType type, const std::string &name, const std::weak_ptr<CodeBlock> &owner)
        : type(type), name(name), owner(owner) {

}

void IR::insertComment(std::ostream &ostream) const {
    if (!comment.empty()) ostream << " ; " << comment;
}

void LabelDefIR::generateIr(std::ostream &ostream) {
    ostream << "LABEL " << this->label << " :";
    insertComment(ostream);
    ostream << std::endl;
}


FunctionDefIR::FunctionDefIR(std::string& identifier) : IR(IRType::FunctionDef) {
    assert(Scope::getGlobalScope()->isSymbolExists(identifier));
    this->functionName = identifier;
    this->functionType = Scope::getGlobalScope()->lookupSymbol(identifier);
    std::ostringstream ss;
    ss << "function " << identifier << *this->functionType << std::endl;
    this->comment = ss.str();
}

void FunctionDefIR::generateIr(std::ostream &ostream) {
    ostream << "FUNCTION " << this->functionName << " :";
    insertComment(ostream);
    ostream << std::endl;
    // generate Params
    for (const auto &item: this->functionType->funcArgs.operator*()) {
        ostream << "PARAM " << item.first << " ; Type:" << item.second << std::endl;
    }
}

AllocateIR::AllocateIR(const size_t size, std::shared_ptr<IRVariable> &variable, std::string &identifierName)
        : IR(IRType::Allocate), variable(variable), size(size) {
    std::ostringstream ss;
    ss << "allocate " << size << " bytes for " << identifierName;
    this->comment = ss.str();
}

void AllocateIR::generateIr(std::ostream &ostream) {
    ostream << "DEC " << variable->name << " " << this->size;
    insertComment(ostream);
    ostream << std::endl;
}