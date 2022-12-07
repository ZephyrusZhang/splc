#ifndef SPLC_IR_H
#define SPLC_IR_H

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include "CompoundType.h"

enum class IRType {
    LabelDef,
    FunctionDef,
    Assign,
    ArithAddition,
    ArithSubtraction,
    ArithMultiplication,
    ArithDivision,
    AddressOf, // x := &y
    ReadAddress, // x := *y
    StoreAddress, // *x := y
    Goto,
    If,
    Return,
    Allocate,
    FunctionParam,
    CallArg,
    FunctionCall,
    BuiltinRead,
    BuiltinWrite,
    CodeBlock
};

/*
 * LABEL x :                define a label x
 * FUNCTION f :             define a function f
 * x := y                   assign value of y to x
 * x := y + z               arithmetic addition
 * x := y - z               arithmetic subtraction
 * x := y * z               arithmetic multiplication
 * x := y / z               arithmetic division
 * x := &y                  assign address of y to x
 * x := *y                  assign value stored in address y to x
 * *x := y                  copy value y to address x
 * GOTO x                   unconditional jump to label x
 * IF x [relop] y GOTO z    if the condition (binary boolean) is true, jump to label z
 * RETURN x                 exit the current function and return value x
 * DEC x [size]             allocate space pointed by x, size must be a multiple of 4
 * PARAM x                  declare a function parameter
 * ARG x                    pass argument x
 * x := CALL f              call a function, assign the return value to x
 * READ x                   read x from console
 * WRITE x                  print the value of x to console
 */

class CodeBlock;
class IR;

enum class IRVariableType {
    Int,
    BaseAddress,
    Pointer,
    ArrayIndex,
    StructOffset
};

class IRVariable : private std::enable_shared_from_this<IRVariable> {
public:
    IRVariableType type;
    const std::string name;
    const std::weak_ptr<CodeBlock> owner;
    std::vector<std::weak_ptr<IR>> references;

    IRVariable(IRVariableType type, std::string name, std::weak_ptr<CodeBlock> owner);
    // Allocate space according to CompoundType
    IRVariable(std::string name, const CompoundType& compoundType, std::weak_ptr<CodeBlock> owner);
};

class IRConstant : private std::enable_shared_from_this<IRVariable> {
public:
    std::string value;
    std::vector<std::weak_ptr<IR>> references;

    explicit IRConstant(std::string value) : value(std::move(value)) {};
};

class IR : public std::enable_shared_from_this<IR> {
public:
    const IRType irType;
    std::string comment;
    std::vector<std::weak_ptr<IR>> references;

    explicit IR(IRType irType) : irType(irType) {};

    virtual void generateIr(std::ostream &ostream) = 0;

protected:
    void insertComment(std::ostream &ostream) const;

    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

public:
    template<typename T>
    std::shared_ptr<T> &castTo() {
        static_assert(std::is_base_of<IR, T>::value, "T should inherit from IR");
        const IRType _irType = T::irType;
        assert(_irType == this->irType);
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }
};

class LabelDefIR : public IR {
public:
    const std::string label;

    explicit LabelDefIR(std::string label) : IR(IRType::LabelDef), label(std::move(label)) {};
    void generateIr(std::ostream &ostream) override;
};

class FunctionDefIR : public IR {
public:
    std::string functionName;
    std::shared_ptr<CompoundType> functionType;

    explicit FunctionDefIR(std::string &identifier);

    void generateIr(std::ostream &ostream) override;
};

class AssignIR : public IR {
    const std::shared_ptr<IRVariable> target;
    const std::shared_ptr<IRConstant> source;;
    void generateIr(std::ostream &ostream) override;

public:
    explicit AssignIR(std::shared_ptr<IRVariable> target, std::shared_ptr<IRConstant> source)
    : IR(IRType::Assign), target(std::move(target)), source(std::move(source)) {}
};

class AllocateIR : public IR {
public:
    const size_t size;
    std::shared_ptr<IRVariable> variable;

    explicit AllocateIR(size_t size, std::shared_ptr<IRVariable>& variable, std::string& identifierName);
    void generateIr(std::ostream &ostream) override;
};

class IfIR : public IR {
public:
    const std::shared_ptr<IRVariable> condition;
    const std::shared_ptr<LabelDefIR> gotoLabel;

    explicit IfIR(std::shared_ptr<IRVariable> condition, std::shared_ptr<LabelDefIR> gotoLabel)
            : IR(IRType::If), condition(std::move(condition)), gotoLabel(std::move(gotoLabel)) {
//        this->gotoLabel->references.push_back(shared_from_base<IR>());
    }

    void generateIr(std::ostream &ostream) override;
};

class GotoIR : public IR {
public:
    const std::shared_ptr<LabelDefIR> gotoLabel;

    explicit GotoIR(std::shared_ptr<LabelDefIR> gotoLabel)
            : IR(IRType::Goto), gotoLabel(std::move(gotoLabel)) {
        assert(this->gotoLabel);
//        this->gotoLabel->references.push_back(shared_from_this());
    }

    void generateIr(std::ostream &ostream) override;
};

#endif //SPLC_IR_H
