#include <sstream>
#include <utility>
#include "IR.h"
#include "Scope.h"

IRVariable::IRVariable(std::string name, IRVariableType type, std::weak_ptr<CodeBlock> owner)
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

IRVariable::IRVariable(int32_t value, std::weak_ptr<CodeBlock> owner) :name("constantVariable"), owner(std::move(owner)) {
    this->type = IRVariableType::Constant;
    this->value = value;
}

void IR::insertComment(std::ostream &ostream) const {
    if (!comment.empty()) ostream << " ; " << comment;
}

void LabelDefIR::generateIr(std::ostream &ostream) {
    ostream << "LABEL " << this->label << " :";
//    insertComment(ostream);
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
    ostream << std::endl;
    ostream << "FUNCTION " << this->functionName << " :";
//    insertComment(ostream);
    ostream << std::endl;
    // generate Params
    for (const auto &item: this->functionType->funcArgs.operator*()) {
        auto id = functionName + "_param_" + item.first;
        ostream << "PARAM " << id << std::endl;
    }
}

void FunctionCallIR::generateIr(std::ostream &ostream) {
    for (const auto &item: args) {
        ostream << "ARG " << item.operator*() << std::endl;
    }
    ostream << returnVar.operator*() << " := CALL " << functionName << std::endl;
}

void AssignIR::generateIr(std::ostream &ostream) {
    ostream << *target << " := " << *op1;
//    insertComment(ostream);
    ostream << std::endl;
}

std::ostream &operator<<(std::ostream &ostream, const IRVariable& irVariable) {
    if (irVariable.type == IRVariableType::Constant)
        ostream << "#" << irVariable.value;
    else ostream << irVariable.name;
    return ostream;
}

std::ostream& operator<<(std::ostream& os, const IRVariableType& type) {
    if (type == IRVariableType::Pointer) os << "ptr";
    else if (type == IRVariableType::Int) os << "i32";
    else if (type == IRVariableType::Param) os << "param";
    else if (type == IRVariableType::BaseAddress) os << "base";
    else if (type == IRVariableType::StructOffset) os << "off";
    else if (type == IRVariableType::ArrayIndex) os << "idx";
    else if (type == IRVariableType::ExpResult) os << "exp";
    else os << "ukn";
    return os;
}

AllocateIR::AllocateIR(const size_t size, std::shared_ptr<IRVariable> &variable, const std::string &identifierName)
        : IR(IRType::Allocate), variable(variable), size(size) {
    std::ostringstream ss;
    ss << "allocate " << identifierName << ": " << size << " bytes, type " << variable->type;
    this->comment = ss.str();
}

void AllocateIR::generateIr(std::ostream &ostream) {
    ostream << "DEC " << variable->name << " " << this->size;
//    insertComment(ostream);
    ostream << std::endl;
    if (this->addrVar->references.size() > 1) {
        this->addrIr->generateIr(ostream);
    }
}

std::ostream& operator<<(std::ostream& os, const IFRelop& relop) {
    switch (relop) {
        case IFRelop::LE: return (os << "<=");
        case IFRelop::LT: return (os << "<");
        case IFRelop::GT: return (os << ">");
        case IFRelop::GE: return (os << ">=");
        case IFRelop::NE: return (os << "!=");
        case IFRelop::EQ: return (os << "==");
    }
    return os;
}

void IfIR::generateIr(std::ostream &ostream) {
    ostream << "IF " << *left << " " << relop << " " << *right << " GOTO " << gotoLabel->label;
//    insertComment(ostream);
    ostream << std::endl;
}

void GotoIR::generateIr(std::ostream &ostream) {
    ostream << "GOTO " << gotoLabel.lock()->label;
//    insertComment(ostream);
    ostream << std::endl;
}

void BinaryIR::generateIr(std::ostream &ostream) {
    ostream << *target << " := " << *op1 << " ";
    if (irType == IRType::ArithAddition) ostream << "+";
    else if (irType == IRType::ArithSubtraction) ostream << "-";
    else if (irType == IRType::ArithMultiplication) ostream << "*";
    else if (irType == IRType::ArithDivision) ostream << "/";
    else ostream << "NOT IMPLEMENTED";
    ostream << " " << *op2 << std::endl;
}

void UnaryIR::generateIr(std::ostream &ostream) {
    if (irType == IRType::StoreAddress) ostream << "*";
    ostream << *target << " := ";
    if (irType == IRType::AddressOf) ostream << "&";
    else if (irType == IRType::ReadAddress) ostream << "*";
    ostream << *op1 << std::endl;
}

void ReturnIR::generateIr(std::ostream &ostream) {
    ostream << "RETURN ";
    ostream << *returnValue;
    ostream << std::endl;
}
