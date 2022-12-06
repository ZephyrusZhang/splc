#ifndef SPLC_IR_H
#define SPLC_IR_H

#include <string>
#include <memory>
#include <vector>
#include "CompoundType.h"

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

class IR : std::enable_shared_from_this<IR> {
public:
    const IRType irType;
    std::string comment;
    std::vector<std::shared_ptr<IR>> references;    // which irs reference me?

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

class LabelDefIR : IR {
    const std::string label;

    explicit LabelDefIR(std::string label) : IR(IRType::LabelDef), label(std::move(label)) {};

    void generateIr(std::ostream &ostream) override;

};

class FunctionDef : IR {
    std::string functionName;
    std::shared_ptr<CompoundType> functionType;

    explicit FunctionDef(std::string &identifier);

    void generateIr(std::ostream &ostream) override;
};

#endif //SPLC_IR_H
