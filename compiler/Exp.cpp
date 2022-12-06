#include "Node.h"
#include "Exp.h"
#include "Container.h"
#include "Scope.h"
#include "Dec.h"
#include "Def.h"
#include "Util.h"
#include <memory>
#include <vector>
#include <algorithm>

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
        if (!CompoundType::canAssignment(*left.expCompoundType, *right.expCompoundType)) {
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                      << *left.expCompoundType
                      << " and " << *right.expCompoundType << " in the assignment." << std::endl;
        }
        this->valueType = ValueType::RValue;
        this->expCompoundType = right.expCompoundType;
    } else if (expType == ExpType::NOT) {
        auto &right = children[1]->container->castTo<Exp>().operator*();
        this->valueType = ValueType::RValue;
        this->expCompoundType = right.expCompoundType;
        if (!right.expCompoundType->canDoBoolean()) {
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type " << *right.expCompoundType
                      << " in the NOT boolean operation." << std::endl;
        }
    } else if (expType == ExpType::AND || expType == ExpType::OR) {
        // Boolean Operation
        // TODO: Type Checking
        auto &left = children[0]->container->castTo<Exp>().operator*();
        auto &right = children[2]->container->castTo<Exp>().operator*();
        this->valueType = ValueType::RValue;
        this->expCompoundType = left.expCompoundType;
        if (!(left.expCompoundType->canDoBoolean() && right.expCompoundType->canDoBoolean())) {
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                      << *left.expCompoundType << " and " << *right.expCompoundType
                      << " in the boolean operation." << std::endl;
        } 
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
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                      << *left.expCompoundType << " and " << *right.expCompoundType
                      << " in the compare operation." << std::endl;
        }
    } else if (expType == ExpType::PLUS || expType == ExpType::MINUS ||
               expType == ExpType::MUL || expType == ExpType::DIV) {
        // Binary Arithmetic Operation
        const Exp &left = children[0]->container->castTo<Exp>().operator*();
        const Exp &right = children[2]->container->castTo<Exp>().operator*();
        this->valueType = ValueType::RValue;
        this->expCompoundType = left.expCompoundType;
        // TODO: do type checking, be careful that left and right are Reference of Exp
        // and Exp&.expCompoundType is a POINTER,
        // You need to dereference it first to get a reference to CompoundType, and use operator== to check type equality
        if (!(*left.expCompoundType == *right.expCompoundType) && (left.expCompoundType->canDoArithmetic() && right.expCompoundType->canDoArithmetic())) {
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                      << *left.expCompoundType << " and " << *right.expCompoundType
                      << " in the arithmetic operation." << std::endl;
        } else if (!(left.expCompoundType->canDoArithmetic() && right.expCompoundType->canDoArithmetic())) {
            std::cerr << "Error type 7 at line " << this->node->lineno << ": binary operation on non-number variables." << std::endl;
        } else {
            if (left.expType == ExpType::LITERAL_INT && right.expType == ExpType::LITERAL_INT) {
                this->expType = ExpType::LITERAL_INT;
                this->integerValue = left.integerValue + right.integerValue;
            }
        }
    } else if (expType == ExpType::INCREASE || expType == ExpType::DECREASE) {
        // Unary Arithmetic Operation
        auto &operand = children[0]->container->castTo<Exp>().operator*();
        this->valueType = ValueType::RValue;
        this->expCompoundType = operand.expCompoundType;
        // TODO: Type Checking for ++ and --
        if (!(operand.expCompoundType->canDoArithmetic())) {
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                      << *operand.expCompoundType << " in self arithmetic operation." << std::endl;
        }
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
                                [](const CompoundType::FuncArg &defArg, const Node *arg) {
                                    return defArg.second == *arg->container->castTo<Exp>()->expCompoundType;
                                })) {
                    std::cerr << "Error type 9 at line " << this->node->lineno << ": function " << id
                              << " is called with mismatched args" << std::endl;
                }
            }
        } else if (Scope::getCurrentScope()->isSymbolExistsRecursively(id)) {
            const auto &variable = Scope::getCurrentScope()->lookupSymbol(id);
            if (variable->funcArgs == NULL) {
                std::cerr << "Error type 11 at line " << this->node->lineno
                          << ": applying function invocation operator on non-function identifier "
                          << "{" << id << "}" << std::endl;
                this->expCompoundType = std::make_shared<CompoundType>(TypeInt);             
            }
        } else {
            int idx = 0;
            extern Node **yystack;
            bool isAssign = false;
            std::vector<Node *> exp;
            Node *cur = yystack[idx];
            while (!cur->hasToken({"SEMI", "LC"})) {
                exp.push_back(cur);
                cur = yystack[--idx];
            }
            std::reverse(exp.begin(), exp.end());

            if (exp[0]->tokenName == "Specifier") {
                this->expCompoundType = std::make_shared<CompoundType>(Specifier::getSpecifierType(exp[0]));
            } else {
                auto ids = Node::convertTreeToVector(exp[0], "", {"ID"}, true);
                if (ids.size() > 0) {
                    auto symbol = Scope::getCurrentScope()->lookupSymbol(ids[0]->data);
                    this->expCompoundType = std::make_shared<CompoundType>(symbol->type);
                } else {
                    this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
                }
            }
            std::cerr << "Error type 2 at line " << this->node->lineno << ": function " << id
                      << " is invoked without definition, guess its return value type is "
                      << *this->expCompoundType << std::endl;
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
                int value = std::stoi(index.node->children[0]->data);
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
        std::string intStr = this->node->children[0]->data;
        extern Node **yystack;
        if (yystack[-1]->tokenName == "MINUS") {
            intStr = "-" + intStr;
        }
        if (isIntStrOverflow(intStr)) {
            std::cerr << "Error at line " << this->node->lineno << ": "
                      << "32-bit integer " << intStr << " overflow." << std::endl;
        } else {
            this->integerValue = std::stoi(intStr);
        }
    } else if (this->expType == ExpType::LITERAL_FLOAT) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypeFloat);
    } else if (this->expType == ExpType::LITERAL_CHAR) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypeChar);
        std::string charStr = this->node->children[0]->data;
        if (isHexCharOverflow(charStr.substr(1, charStr.size() - 2))) {
            std::cerr << "Error at line " << this->node->lineno << ": "
                      << "hex char overflow." << std::endl;
        }
    } else if (this->expType == ExpType::LITERAL_STRING) {
        this->valueType = ValueType::RValue;
        this->expCompoundType = std::make_shared<CompoundType>(TypePointer);
        this->expCompoundType->pointTo = std::make_shared<CompoundType>(TypeChar);
    } else if (expType == ExpType::DOT_ACCESS || expType == ExpType::PTR_ACCESS) {
        this->valueType = ValueType::LValue;
        auto *leftType = children[0]->container->castTo<Exp>().operator*().expCompoundType.get();
        auto &id = children[2]->data;
        if (expType == ExpType::PTR_ACCESS) {
            if (leftType->type == TypePointer && leftType->pointTo->type == TypeStruct)
                leftType = leftType->pointTo.get();
            else {
                std::cerr << "Error type 13 at line " << this->node->lineno
                          << ": accessing members of a non-structure pointer." << std::endl;
            }
        }
        if (leftType->type == TypeStruct) {
            const auto &defList = *leftType->structDefLists;
            const auto findId = [&id](const CompoundType::StructDefList &defList) { return defList.first == id; };
            const auto &result = std::find_if(defList.begin(), defList.end(), findId);
            if (result == defList.end()) {
                std::cerr << "Error type 14 at line " << this->node->lineno <<
                          ": accessing an undefined structure member " << id << " at " << *leftType
                          << std::endl;
                this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
            } else {
                this->expCompoundType = std::make_shared<CompoundType>(result->second);
                // warning: copy of CompoundType
            }
        } else {
            std::cerr << "Error type 13 at line " << this->node->lineno
                      << ": accessing members of a non-structure variable." << std::endl;
            this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
        }
    } else if (expType == ExpType::ADDRESS_OF) {
        this->valueType = ValueType::RValue;
        auto &rightExp = children[1]->container->castTo<Exp>().operator*();
        if (rightExp.valueType == ValueType::RValue) {
            std::cerr << "Error type ? at line " << this->node->lineno
                      << ": cannot get address of rvalue expression." << std::endl;
        }
        this->expCompoundType = std::make_shared<CompoundType>(TypePointer);
        this->expCompoundType->pointTo = rightExp.expCompoundType;
    } else if (expType == ExpType::DEREF) {
        this->valueType = ValueType::LValue;
        auto &ptrExp = children[1]->container->castTo<Exp>().operator*();
        if (ptrExp.expCompoundType->type == TypePointer) {
            this->expCompoundType = ptrExp.expCompoundType->pointTo;
        } else {
            std::cerr << "Error type ? at line " << this->node->lineno
                      << ": cannot dereference of a non-pointer" << std::endl;
            this->expCompoundType = std::make_shared<CompoundType>(TypeInt);
        }
    } else if (expType == ExpType::TYPE_CAST) {
        auto &specifier = children[1]->container->castTo<Specifier>().operator*();
        auto &castedExp = children[3]->container->castTo<Exp>().operator*();
        this->expCompoundType = std::make_shared<CompoundType>(specifier);
        this->valueType = castedExp.valueType;
        // TODO: Check Casting
        if (!((specifier.type == TypeFloat && (castedExp.expCompoundType->type == TypeInt || castedExp.expCompoundType->type == TypeFloat)) ||
              (specifier.type == TypeInt && (castedExp.expCompoundType->type == TypeFloat || castedExp.expCompoundType->type == TypeInt)) ||
              (specifier.type == TypeChar && (castedExp.expCompoundType->type == TypeInt || castedExp.expCompoundType->type == TypeChar)) ||
              (specifier.type == TypeInt && (castedExp.expCompoundType->type == TypeChar || castedExp.expCompoundType->type == TypeInt)))) {
            std::cerr << "Error type 5 at line " << this->node->lineno << ": unmatched type "
                      << specifier << " and " << *castedExp.expCompoundType
                      << " in the force cast operation." << std::endl;
        }
    } else if (expType == ExpType::NEGATIVE_SIGN) {
        auto& right = children[1]->container->castTo<Exp>().operator*();
        this->expCompoundType = right.expCompoundType;
        this->valueType = ValueType::RValue;
        // TODO: Check can do arithmetic
    } else throw std::runtime_error("unexpected ExpType");
    // assert all properties are set
    assert(this->valueType != ValueType::Unknown);
    assert(this->expCompoundType != nullptr);
}

const CompoundType &Exp::getCompoundType() const {
    return *expCompoundType;
}
