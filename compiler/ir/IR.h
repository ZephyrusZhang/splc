#ifndef SPLC_IR_H
#define SPLC_IR_H

#include <string>
#include <memory>
#include <vector>
#include "../CompoundType.h"

enum IRType {
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
    BuiltinWrite
};

class CodeBlock;
class IR;

enum IRVariableType {
    Int,
    BaseAddress,
    ArrayIndex,
    StructOffset
};
// i0 := *ptr
// b0 := i0 + 1
//
// a[*ptr+1]
class IRVariable : private std::enable_shared_from_this<IRVariable> {
public:
    const IRVariableType type;
    const std::string name;
    const std::weak_ptr<CodeBlock> owner;
    std::vector<std::weak_ptr<IR>> references;

    IRVariable(const IRVariableType type, const std::string &name, const std::weak_ptr<CodeBlock> &owner);
};

class IR : private std::enable_shared_from_this<IR> {
public:
    const IRType irType;
    std::string comment;
    std::shared_ptr<IRVariable> target;
    std::shared_ptr<IRVariable> op1;
    std::shared_ptr<IRVariable> op2;

    explicit IR(IRType irType) : irType(irType) {};

    virtual void generateIr(std::ostream &ostream) = 0;

protected:
    void insertComment(std::ostream &ostream) const;

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

class AllocateIR : public IR {
public:
    const size_t size;
    const std::shared_ptr<IRVariable> variable;

    explicit AllocateIR(const size_t size, std::shared_ptr<IRVariable>& variable, std::string& identifierName);
    void generateIr(std::ostream &ostream) override;
};

#endif //SPLC_IR_H
