#include <sstream>
#include <utility>
#include "IR.h"
#include "Scope.h"

IRVariable::IRVariable(IRVariableType type, std::string name, std::weak_ptr<CodeBlock> owner)
        : type(type), name(std::move(name)), owner(std::move(owner)) {

}

IRVariable::IRVariable(std::string name, const CompoundType& compoundType, std::weak_ptr<CodeBlock> owner) : name(std::move(name)), owner(std::move(owner)) {
    switch (compoundType.type) {
        case BasicType::TypeStruct: type = IRVariableType::BaseAddress; break;
        case BasicType::TypePointer: type = IRVariableType::Pointer; break;
        default: type = IRVariableType::Pointer;
    }
    if (compoundType.type == BasicType::TypeStruct) {
        type = IRVariableType::BaseAddress;
    } else if (compoundType.type == BasicType::TypePointer) {
        if (compoundType.isArray()) type = IRVariableType::BaseAddress;
        else type = IRVariableType::Pointer;
    } else type = IRVariableType::Pointer;
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
    ss << "function " << identifier;
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

void AssignIR::generateIr(std::ostream &ostream) {
    ostream << target->name << " := " << source->value;
    insertComment(ostream);
    ostream << std::endl;
}

std::ostream& operator<<(std::ostream& os, const IRVariableType& type) {
    if (type == IRVariableType::Pointer) os << "ptr";
    else if (type == IRVariableType::Int) os << "i32";
    else if (type == IRVariableType::BaseAddress) os << "base";
    else if (type == IRVariableType::StructOffset) os << "off";
    else if (type == IRVariableType::ArrayIndex) os << "idx";
}

AllocateIR::AllocateIR(const size_t size, std::shared_ptr<IRVariable> &variable, std::string &identifierName)
        : IR(IRType::Allocate), variable(variable), size(size) {
    std::ostringstream ss;
    ss << "allocate " << identifierName << ": " << size << " bytes, type " << variable->type;
    this->comment = ss.str();
}

void AllocateIR::generateIr(std::ostream &ostream) {
    ostream << "DEC " << variable->name << " " << this->size;
    insertComment(ostream);
    ostream << std::endl;
}

void IfIR::generateIr(std::ostream &ostream) {
    ostream << "IF " << condition->name << " == 0 GOTO " << gotoLabel->label;
    insertComment(ostream);
    ostream << std::endl;
}

void GotoIR::generateIr(std::ostream &ostream) {
    ostream << "GOTO " << gotoLabel->label;
    insertComment(ostream);
    ostream << std::endl;
}