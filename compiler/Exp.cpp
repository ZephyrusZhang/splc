#include "Node.h"
#include "Exp.h"
#include "Container.h"
#include "Scope.h"
#include "Dec.h"
#include "Def.h"
#include <memory>
#include <vector>

Exp::Exp(Node *node, ExpType expType)
        : Container(node, containerType) {
    this->expType = expType;
}

void Exp::installChild(const std::vector<Node *> &children) {
    if (expType == ExpType::ASSIGN) {
        // Assignment
        auto &left = children[0]->container->castTo<Exp>().operator*();
        auto &right = children[2]->container->castTo<Exp>().operator*();
        if (left.valueType == ValueType::RValue) {
            std::cerr << "Error type 6 at line " << this->node->lineno << ": rvalue type " << *left.expCompoundType
                      << " appears in the left side of assignment." << std::endl;
        }
        if (!(left.expCompoundType == right.expCompoundType)) {
            if (left.expCompoundType->type == TypePointer && right.expCompoundType->type == TypePointer
                && right.expCompoundType->pointTo->type == TypeVoid) {
                // allow assignment from void* to any pointer type
            } else {
                std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                          << *left.expCompoundType
                          << " and " << *right.expCompoundType << "in the assignment." << std::endl;
            }
        }
        this->valueType = ValueType::RValue;
        this->expCompoundType = right.expCompoundType;
    } else if (expType == ExpType::AND || expType == ExpType::OR || expType == ExpType::NOT) {
        // Boolean Operation
        this->valueType == ValueType::RValue;
        // TODO: Type Checking
    } else if (expType == ExpType::LT || expType == ExpType::LE
               || expType == ExpType::GT || expType == ExpType::GE
               || expType == ExpType::NE || expType == ExpType::EQ) {
        // Arithmetic Operation
        auto &left = children[0]->container->castTo<Exp>().operator*();
        auto &right = children[2]->container->castTo<Exp>().operator*();
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(BasicType::TypeInt);
        // convert current type to Int
        if (!(*left.expCompoundType == *right.expCompoundType && left.expCompoundType->canCompare())) {
            // TODO: Type Checking
        }
    } else if (expType == ExpType::PLUS || expType == ExpType::MINUS ||
               expType == ExpType::MUL || expType == ExpType::DIV) {
        // Binary Arithmetic Operation
        const Exp &left = children[0]->container->castTo<Exp>().operator*();
        const Exp &right = children[2]->container->castTo<Exp>().operator*();
        if (*left.expCompoundType == *right.expCompoundType && left.expCompoundType->canDoArithmetic())
            this->expCompoundType = left.expCompoundType;
        // TODO: do type checking, be careful that left and right are Reference of Exp
        // and Exp&.expCompoundType is a POINTER,
        // You need to dereference it first to get a reference to CompoundType, and use operator== to check type equality
    } else if (expType == ExpType::INCREASE || expType == ExpType::DECREASE) {
        // Unary Arithmetic Operation
        auto &operand = children[1]->container->castTo<Exp>().operator*();
        this->valueType = ValueType::LValue;
        this->expCompoundType = operand.expCompoundType;
        // TODO: Type Checking for ++ and --
    } else if (expType == ExpType::SCOPE) {
        auto &operand = children[1]->container->castTo<Exp>().operator*();
        this->expCompoundType = operand.expCompoundType;
        this->valueType = operand.valueType;
    } else if (this->expType == ExpType::FUNC_INVOKE) {
        this->valueType = ValueType::RValue;
        const auto &id = children[0]->data;
        if (Scope::getGlobalScope()->isSymbolExists(id)) {
            const auto &func = Scope::getGlobalScope()->lookupSymbol(id);
            this->expCompoundType = func; // set return type
            if (children.size() == 4) { // check args type
                assert(children[2]->tokenName == "Args");
                auto callArgs = Node::convertTreeToVector(children[2], "Args", {"Exp"});
                const auto &defArgs = func->funcArgs.operator*();
                if (!std::equal(defArgs.begin(), defArgs.end(), callArgs.begin(), callArgs.end(),
                                [](const CompoundType &defArg, const Node *arg) {
                                    return defArg == *arg->container->castTo<Exp>()->expCompoundType;
                                })) {
                    std::cerr << "Error type 9 at line " << this->node->lineno << ": function " << id
                              << "is called with mismatched args" << std::endl;
                }
            }
        } else {
            std::cerr << "Error type 2 at line " << this->node->lineno << ": function " << id
                      << " is invoked without definition" << std::endl;
        }
    } else if (this->expType == ExpType::ARRAY_INDEX) {
        this->valueType = ValueType::LValue;
        auto &array = children[0]->container->castTo<Exp>().operator*();
        auto &index = children[2]->container->castTo<Exp>().operator*();
        if (array.expCompoundType->type != TypePointer) {
            std::cerr << "Error type 10 at line " << this->node->lineno << ": index at a non-pointer/non-array object"
                      << std::endl;
            this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
            // set to int to make other checking works
        } else {
            this->expCompoundType = array.expCompoundType->pointTo;
        }
        if (index.expCompoundType->type != TypeInt) {
            std::cerr << "Error type 12 at line " << this->node->lineno
                      << ": array indexing with a non-integer type expression"
                      << std::endl;
        } else {
            if (array.expCompoundType->maxIndex > 0 && index.expType == ExpType::LITERAL_INT) {
                int value = std::stoi(index.node->data);
                if (value >= array.expCompoundType->maxIndex) {
                    std::cerr << "Warning: index out of bound at line " << this->node->lineno << " ,maxSize "
                              << array.expCompoundType->maxIndex << " but given " << value << "." << std::endl;
                }
            }
        }
    } else if (this->expType == ExpType::IDENTIFIER) {
        this->valueType = ValueType::LValue;
        const auto &id = children[0]->data;
        if (!Scope::getCurrentScope()->isSymbolExistsRecursively(id)) {
            std::cerr << "Error type 1 at line " << this->node->lineno << ": variable " << id
                      << " is used without definition, guess its type to int" << std::endl;
            this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
        } else {
            this->expCompoundType = Scope::getCurrentScope()->lookupSymbol(id);
        }
    } else if (this->expType == ExpType::LITERAL_INT) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
    } else if (this->expType == ExpType::LITERAL_FLOAT) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypeFloat);
    } else if (this->expType == ExpType::LITERAL_CHAR) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypeChar);
    } else if (this->expType == ExpType::LITERAL_STRING) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypePointer);
        this->expCompoundType->pointTo = std::make_shared<CompoundType>(TypeChar);
    } else if (expType == ExpType::DOT_ACCESS) {

    } else if (expType == ExpType::PTR_ACCESS) {

    } else if (expType == ExpType::ADDRESS_OF) {

    } else if (expType == ExpType::DEREF) {

    } else if (expType == ExpType::TYPE_CAST) {
        auto &specifier = children[1]->container->castTo<Specifier>().operator*();
        this->expCompoundType = std::make_shared<CompoundType>(specifier);
        // TODO: Check Casting
    } else throw std::runtime_error("unexpected ExpType ");
    // assert all properties are set
    assert(this->valueType != ValueType::Unknown);
    assert(this->expCompoundType != nullptr);
}
