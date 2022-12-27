#ifndef SPLC_IR_H
#define SPLC_IR_H

#define PARAM_PREFIX "__p_"

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
    Param,
    ArrayIndex,
    StructOffset,
    Temp,
    Constant,
    ExpResult,
};

class IRVariable : public std::enable_shared_from_this<IRVariable> {
public:
    IRVariableType type;
    const std::string name;
    const std::weak_ptr<CodeBlock> owner;
    std::vector<std::weak_ptr<IR>> references;
    int32_t value = 0;

    IRVariable(std::string name, IRVariableType type, std::weak_ptr<CodeBlock> owner);

    // Allocate space according to CompoundType
    IRVariable(std::string name, const CompoundType &compoundType, std::weak_ptr<CodeBlock> owner);

    // Constant Value
    IRVariable(int32_t value, std::weak_ptr<CodeBlock> owner);

    friend std::ostream& operator<<(std::ostream& ostream, const IRVariable& irVariable);
};

typedef std::shared_ptr<IRVariable> IRVariablePtr;
typedef std::shared_ptr<IR> IRPtr;

class IR : public std::enable_shared_from_this<IR> {
public:
    const IRType irType;
    std::string comment;
    std::vector<std::weak_ptr<IR>> references;

    explicit IR(IRType irType) : irType(irType) {};

    virtual void generateIr(std::ostream &ostream) = 0;

    virtual ~IR() = default;

protected:
    void insertComment(std::ostream &ostream) const;

    template<class Derived>
    std::shared_ptr<Derived> shared_from_base() {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

public:
    template<typename T>
    std::shared_ptr<T> castTo() {
        assert(this);
        static_assert(std::is_base_of<IR, T>::value, "T should inherit from IR");
        assert(typeid(this) == typeid(T));
        return std::dynamic_pointer_cast<T>(shared_from_this());
    }

    virtual void countReference() {}
};

class LabelDefIR : public IR {
public:
    const std::string label;

    explicit LabelDefIR(std::string label) : IR(IRType::LabelDef), label(std::move(label)) {};

    void generateIr(std::ostream &ostream) override;

    ~LabelDefIR() override = default;
};

class FunctionDefIR : public IR {
public:
    std::string functionName;
    std::shared_ptr<CompoundType> functionType;

    explicit FunctionDefIR(std::string &identifier);

    void generateIr(std::ostream &ostream) override;

    ~FunctionDefIR() override = default;
};

class FunctionCallIR: public IR {
public:
    std::string functionName;
    std::vector<IRVariablePtr> args;
    IRVariablePtr returnVar;

    FunctionCallIR(std::string functionName, std::vector<IRVariablePtr> args, IRVariablePtr returnVar)
                   : IR(IRType::FunctionCall), functionName(std::move(functionName)), args(std::move(args)),
                                                                   returnVar(std::move(returnVar)) {}

    ~FunctionCallIR() override = default;

    void generateIr(std::ostream &ostream) override;

    void countReference() override {
        for (auto &item: args) item->references.push_back(shared_from_this());
    }
};

class ReadIR : public IR {
public:
    IRVariablePtr src;

    explicit ReadIR(IRVariablePtr src) : IR(IRType::BuiltinRead), src(std::move(src)) {}
    ~ReadIR() override = default;

    void countReference() override {
        src->references.push_back(shared_from_this());
    }

    void generateIr(std::ostream &ostream) override {
        ostream << "READ " << *src << std::endl;
    }
};

class WriteIR : public IR {
public:
    IRVariablePtr dst;
    explicit WriteIR(IRVariablePtr dst) : IR(IRType::BuiltinWrite), dst(std::move(dst)) {}

    void generateIr(std::ostream &ostream) override {
        ostream << "WRITE " << *dst << std::endl;
    }

    ~WriteIR() override = default;

    void countReference() override {
        dst->references.push_back(shared_from_this());
    }
};

class BinaryIR : public IR {
public:
    IRVariablePtr target;
    IRVariablePtr op1;
    IRVariablePtr op2;

    BinaryIR(IRType irType, IRVariablePtr target, IRVariablePtr op1, IRVariablePtr op2)
            : IR(irType), target(std::move(target)), op1(std::move(op1)), op2(std::move(op2)) {}

    void generateIr(std::ostream &ostream) override;

    ~BinaryIR() override = default;

    void countReference() override {
        target->references.push_back(shared_from_this());
        op1->references.push_back(shared_from_this());
        op2->references.push_back(shared_from_this());
    }
};

class UnaryIR : public IR {
public:
    IRVariablePtr target;
    IRVariablePtr op1;

    UnaryIR(IRType irType, IRVariablePtr target, IRVariablePtr op1)
            : IR(irType), target(std::move(target)), op1(std::move(op1)) {}

    void generateIr(std::ostream &ostream) override;

    ~UnaryIR() override = default;

    void countReference() override {
        target->references.push_back(shared_from_this());
        op1->references.push_back(shared_from_this());
    }
};

class AssignIR : public UnaryIR {
public:
    explicit AssignIR(IRVariablePtr target, IRVariablePtr source)
            : UnaryIR(IRType::Assign, std::move(target), std::move(source)) {}

    void generateIr(std::ostream &ostream) override;

    ~AssignIR() override = default;
};

class AddressOfIR : public UnaryIR {
public:
    explicit AddressOfIR(IRVariablePtr target, IRVariablePtr op1)
            : UnaryIR(IRType::AddressOf, std::move(target), std::move(op1)) {}

    ~AddressOfIR() override = default;
};

class ReadAddressIR : public UnaryIR {
public:
    explicit ReadAddressIR(IRVariablePtr target, IRVariablePtr srcAddr)
            : UnaryIR(IRType::ReadAddress, std::move(target), std::move(srcAddr)) {}

    ~ReadAddressIR() override = default;
};

class StoreAddressIR : public UnaryIR {
public:
    explicit StoreAddressIR(IRVariablePtr dstAddr, IRVariablePtr value)
            : UnaryIR(IRType::StoreAddress, std::move(dstAddr), std::move(value)) {}

    ~StoreAddressIR() override = default;
};

class AdditionIR : public BinaryIR {
public:
    explicit AdditionIR(IRVariablePtr target, IRVariablePtr op1, IRVariablePtr op2)
            : BinaryIR(IRType::ArithAddition, std::move(target), std::move(op1), std::move(op2)) {}

    ~AdditionIR() override = default;
};

class SubtractionIR : public BinaryIR {
public:
    explicit SubtractionIR(IRVariablePtr target, IRVariablePtr op1, IRVariablePtr op2)
            : BinaryIR(IRType::ArithSubtraction, std::move(target), std::move(op1), std::move(op2)) {}

    ~SubtractionIR() override = default;
};

class MultiplicationIR : public BinaryIR {
public:
    explicit MultiplicationIR(IRVariablePtr target, IRVariablePtr op1, IRVariablePtr op2)
            : BinaryIR(IRType::ArithMultiplication, std::move(target), std::move(op1), std::move(op2)) {}

    ~MultiplicationIR() override = default;
};

class DivisionIR : public BinaryIR {
public:
    explicit DivisionIR(IRVariablePtr target, IRVariablePtr op1, IRVariablePtr op2)
            : BinaryIR(IRType::ArithDivision, std::move(target), std::move(op1), std::move(op2)) {}

    ~DivisionIR() override = default;
};

class AllocateIR : public IR {
public:
    const size_t size;
    IRVariablePtr variable;

    explicit AllocateIR(size_t size, IRVariablePtr &variable, const std::string &identifierName);

    void generateIr(std::ostream &ostream) override;

    ~AllocateIR() override = default;

    void countReference() override {
        variable->references.push_back(shared_from_this());
    }
};

// IF x [relop] y GOTO label
enum class IFRelop {
    /* <  */    LT,
    /* <= */    LE,
    /* >  */    GT,
    /* >= */    GE,
    /* != */    NE,
    /* == */    EQ,
};

class IfIR : public IR {
public:
    IRVariablePtr left;
    IFRelop relop;
    IRVariablePtr right;
    std::shared_ptr<LabelDefIR> gotoLabel;

    explicit IfIR(IRVariablePtr left, IFRelop relop, IRVariablePtr right, std::shared_ptr<LabelDefIR> gotoLabel)
            : IR(IRType::If), left(std::move(left)), relop(relop), right(std::move(right)), gotoLabel(std::move(gotoLabel)) {
    }

    void generateIr(std::ostream &ostream) override;

    ~IfIR() override = default;

    void countReference() override {
        left->references.push_back(shared_from_this());
        right->references.push_back(shared_from_this());
        gotoLabel->references.push_back(shared_from_this());
    }
};

class GotoIR : public IR {
public:
    const std::weak_ptr<LabelDefIR> gotoLabel;

    explicit GotoIR(const std::shared_ptr<LabelDefIR> &gotoLabel)
            : IR(IRType::Goto), gotoLabel(gotoLabel) {

    }

    void generateIr(std::ostream &ostream) override;

    ~GotoIR() override = default;

    void countReference() override {
        gotoLabel.lock()->references.push_back(shared_from_this());
    }
};

class ReturnIR : public IR {
public:
    const std::shared_ptr<IRVariable> returnValue;

    explicit ReturnIR(std::shared_ptr<IRVariable> returnValue)
            : IR(IRType::Return), returnValue(std::move(returnValue)) {

    }

    void generateIr(std::ostream &ostream) override;
    ~ReturnIR() override = default;

    void countReference() override {
        returnValue->references.push_back(shared_from_this());
    }
};

#endif //SPLC_IR_H
