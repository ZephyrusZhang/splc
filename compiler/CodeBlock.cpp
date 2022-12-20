#include <sstream>
#include <utility>
#include "CodeBlock.h"
#include "Node.h"
#include "Def.h"
#include "Stmt.h"
#include "Exp.h"

CodeBlock::CodeBlock(CodeBlockType type, Node *rootNode, const std::shared_ptr<CodeBlock> &parentBlock)
        : IR(IRType::CodeBlock), codeBlockType(type), rootNode(rootNode), parentBlock(parentBlock) {}

std::shared_ptr<IRVariable> CodeBlock::newVariable(IRVariableType expectedType = IRVariableType::Temp) {
    size_t cnt = ++(this->variableCounts[expectedType]);
    std::ostringstream name;
    if (expectedType == IRVariableType::Int) name << 'i';
    else if (expectedType == IRVariableType::ArrayIndex) name << 'x';
    else if (expectedType == IRVariableType::Pointer) name << "p";
    else if (expectedType == IRVariableType::BaseAddress) name << 'b';
    else if (expectedType == IRVariableType::StructOffset) name << 'o';
    else if (expectedType == IRVariableType::Temp) name << "t";
    else if (expectedType == IRVariableType::ExpResult) name << "e";
    else name << "u";
    name << cnt;
    auto var = std::make_shared<IRVariable>(name.str(), expectedType, shared_from_base<CodeBlock>());
    this->variables.push_back(var);
    return var;
}

std::shared_ptr<IRVariable> CodeBlock::constantVariable(int32_t value) {
    auto var = std::make_shared<IRVariable>(value, shared_from_base<CodeBlock>());
    return var;
}

std::shared_ptr<LabelDefIR> CodeBlock::newLabel(LabelType type, int lineno) {
    std::stringstream ss;
    if (type == LabelType::IF_False) ss << "IF_F_";
    else if (type == LabelType::IF_True) ss << "IF_T_";
    else if (type == LabelType::IF_END) ss << "IF_E_";
    else if (type == LabelType::LOOP_END) ss << "FOR_END_";
    else if (type == LabelType::LOOP_BLOCK) ss << "FOR_BLK_";
    else if (type == LabelType::LOOP_CONDITION) ss << "FOR_COND_";
    else if (type == LabelType::IF_COND_SHORTCUT) ss << "IF_STC_";
    else ss << "UKN_" << (int) type;
    ss << ++(this->labelCounts[type]) << "_" << lineno;
    auto label = std::make_shared<LabelDefIR>(ss.str());
    this->labels.push_back(label);
    return label;
}

std::shared_ptr<AllocateIR> CodeBlock::newAllocatedVariable(const std::string &identifier) {
    if (this->allocatedVariables.find(identifier) != this->allocatedVariables.end()) {
        throw std::runtime_error("identifier is already allocated");
    }
    auto &cType = this->currentScope->lookupSymbol(identifier).operator*();
    auto irVar = std::make_shared<IRVariable>(identifier, cType, shared_from_base<CodeBlock>());
    auto allocateIr = newIR<AllocateIR>(cType.sizeOf(), irVar, identifier);
    this->variables.push_back(irVar);
    this->allocatedVariables[identifier] = irVar;
    return allocateIr;
}

std::shared_ptr<IRVariable> CodeBlock::getAllocatedVariable(const std::string &identifier) {
    if (this->allocatedVariables.find(identifier) == this->allocatedVariables.end()) {
        auto parent = this->parentBlock.lock();
        if (parent)
            return parent->getAllocatedVariable(identifier);
        else
            throw std::runtime_error("unallocated identifier when trying to get IRVariable");
    } else {
        return this->allocatedVariables[identifier];
    }
}

void CodeBlock::generateIr(std::ostream &ostream) {
    for (const auto &item: this->content)
        item->generateIr(ostream);
}

std::shared_ptr<IRVariable> CodeBlock::translateAddressExp(std::shared_ptr<Exp> exp, CodeBlockVector &target) {
    assert(exp->getValueType() == ValueType::LValue);
    if (exp->expType == ExpType::IDENTIFIER) {
        const auto &id = exp->getChildData(0);
        return this->getAllocatedVariable(id);
    } else if (exp->expType == ExpType::DOT_ACCESS) {
        auto leftExp = exp->getChildExp(0);
        const auto &id = exp->getChildData(2);
        // calculate the offset of id corresponding to struct
        int32_t offset = leftExp->getCompoundType().getStructOffset(id);
        // get the base address of the left struct
        IRVariablePtr leftBaseAddr = translateExp(leftExp->node, target);
        if (offset == 0) return leftBaseAddr;
        IRVariablePtr retAddr = newVariable(IRVariableType::StructOffset);
        // generate IR: retAddr := leftBaseAddr + offset
        target.push_back(newIR<AdditionIR>(retAddr, leftBaseAddr, constantVariable(offset)));
        return retAddr;
    } else if (exp->expType == ExpType::PTR_ACCESS) {
        auto leftExp = exp->getChildExp(0);
        const auto &id = exp->getChildData(2);
        // calculate the offset of id corresponding to struct
        int32_t offset = leftExp->getCompoundType().pointTo->getStructOffset(id);
        // get the value of the ptr, i.e. the base address of structure
        IRVariablePtr leftBaseAddr = translateExp(leftExp->node, target);
        if (offset == 0) return leftBaseAddr;
        IRVariablePtr retAddr = newVariable(IRVariableType::StructOffset);
        // generate IR: retAddr := leftBaseAddr + offset
        target.push_back(newIR<AdditionIR>(retAddr, leftBaseAddr, constantVariable(offset)));
        return retAddr;
    } else if (exp->expType == ExpType::ARRAY_INDEX) {
        auto arrayExp = exp->getChildExp(0);
        auto indexExp = exp->getChildExp(2);
        assert(arrayExp->getCompoundType().isArray());
        IRVariablePtr leftBaseAddr = translateExp(arrayExp->node, target);
        IRVariablePtr retAddr = newVariable(IRVariableType::StructOffset);
        IRVariablePtr indexVar = translateExp(indexExp->node, this->content);
        IRVariablePtr offsetVar = newVariable(IRVariableType::ArrayIndex);
        target.push_back(newIR<MultiplicationIR>(offsetVar, indexVar,
                                                 constantVariable(arrayExp->getCompoundType().pointTo->sizeOf())));
        target.push_back(newIR<AdditionIR>(retAddr, leftBaseAddr, offsetVar));
        return retAddr;
    } else if (exp->expType == ExpType::DEREF) {
        auto ptrExp = exp->getChildExp(1);
        return translateExp(ptrExp->node, target);
    } else throw std::runtime_error("unsupported operations when trying to calculate exp address");
}

std::shared_ptr<IRVariable> CodeBlock::translateLogicalExp(Node *expRoot, CodeBlockVector &target,
                                                           std::shared_ptr<LabelDefIR> shortCutLabel = nullptr) {
    // Logical Exp: AND, OR, NOT
    auto exp = expRoot->container->castTo<Exp>();
    if (!shortCutLabel) shortCutLabel = newLabel(LabelType::IF_COND_SHORTCUT, expRoot->lineno);
    if (exp->expType == ExpType::AND) {
        auto leftExp = exp->getChildExp(0);
        auto rightExp = exp->getChildExp(2);
        // === Translate a && b =>
        // finalResult = compute(left)
        // IF finalResult == 0 GOTO shortcut
        //   rightResult = compute(right)
        //   finalResult = rightResult      // this breaks SSA. rightResult should be eliminated.
        // LABEL shortcut:
        // ===
        // finalResult = compute(left)
        auto finalResult = translateLogicalExp(leftExp->node, target, shortCutLabel);
        // IF finalResult == 0 GOTO ShortCut
        target.push_back(newIR<IfIR>(finalResult, IFRelop::EQ, constantVariable(0), shortCutLabel));
        //   rightResult = compute(right)
        auto rightResult = translateLogicalExp(rightExp->node, target, shortCutLabel);
        //   finalResult = rightResult
        target.push_back(newIR<AssignIR>(finalResult, rightResult));
        // LABEL ShortCut:
        if (!expRoot->parent || expRoot->parent->tokenName != "Exp"
            || exp->expType != expRoot->parent->container->castTo<Exp>()->expType)
            target.push_back(shortCutLabel);
        return finalResult;
    } else if (exp->expType == ExpType::OR) {
        auto leftExp = exp->getChildExp(0);
        auto rightExp = exp->getChildExp(2);
        // === Translate a || b =>
        // finalResult = compute(left)
        // IF finalResult != 0 GOTO shortcut
        //   rightResult = compute(right)
        //   finalResult = rightResult        // this breaks SSA. rightResult should be eliminated.
        // LABEL shortcut:
        // ===
        // finalResult = compute(left)
        auto finalResult = translateLogicalExp(leftExp->node, target);
        // IF finalResult != 0 GOTO ShortCut
        target.push_back(newIR<IfIR>(finalResult, IFRelop::NE, constantVariable(0), shortCutLabel));
        //   rightResult = compute(right)
        auto rightResult = translateLogicalExp(rightExp->node, target);
        //   finalResult = rightResult
        target.push_back(newIR<AssignIR>(finalResult, rightResult));
        // LABEL ShortCut:
        if (!expRoot->parent || expRoot->parent->tokenName != "Exp"
            || exp->expType != expRoot->parent->container->castTo<Exp>()->expType)
            target.push_back(shortCutLabel);
        return finalResult;
    } else if (exp->expType == ExpType::NOT) {
        // Translate ! exp =>
        auto rightVar = translateExp(exp->getChildAt(1), target);
        // TODO: Finish NOT
    }
    return translateExp(expRoot, target);
}

std::shared_ptr<IRVariable> CodeBlock::translateExp(Node *expRoot, CodeBlockVector &target) {
    auto exp = expRoot->container->castTo<Exp>();
    if (exp->expType == ExpType::PLUS || exp->expType == ExpType::MINUS
        || exp->expType == ExpType::MUL || exp->expType == ExpType::DIV) {
        auto leftVar = translateExp(exp->getChildAt(0), target);
        auto rightVar = translateExp(exp->getChildAt(2), target);
        auto resultVar = newVariable();
        if (exp->expType == ExpType::PLUS)
            target.push_back(newIR<AdditionIR>(resultVar, leftVar, rightVar));
        else if (exp->expType == ExpType::MINUS)
            target.push_back(newIR<SubtractionIR>(resultVar, leftVar, rightVar));
        else if (exp->expType == ExpType::MUL)
            target.push_back(newIR<MultiplicationIR>(resultVar, leftVar, rightVar));
        else if (exp->expType == ExpType::DIV)
            target.push_back(newIR<DivisionIR>(resultVar, leftVar, rightVar));
        return resultVar;
    } else if (exp->expType == ExpType::IDENTIFIER || exp->expType == ExpType::DOT_ACCESS ||
               exp->expType == ExpType::PTR_ACCESS || exp->expType == ExpType::ARRAY_INDEX ||
               exp->expType == ExpType::DEREF) {
        auto addrVar = translateAddressExp(exp, target);
        auto valueVar = newVariable();
        target.push_back(newIR<ReadAddressIR>(valueVar, addrVar));
        return valueVar;
    } else if (exp->expType == ExpType::AND || exp->expType == ExpType::OR || exp->expType == ExpType::NOT) {
        return translateLogicalExp(expRoot, target);
    } else if (exp->expType == ExpType::ASSIGN) {
        auto addrVar = translateAddressExp(exp->getChildExp(0), target);
        auto valueVar = translateExp(exp->getChildAt(2), target);
        target.push_back(newIR<StoreAddressIR>(addrVar, valueVar));
        return valueVar;
    } else if (exp->expType == ExpType::LITERAL_INT) {
        return constantVariable(exp->getIntegerValue());
    } else if (exp->expType == ExpType::LITERAL_FLOAT) {
        std::cout << "float is not supported." << std::endl;
        return constantVariable(-114514);
    } else if (exp->expType == ExpType::LITERAL_CHAR) {
        return constantVariable(std::stoi(exp->getChildData(0)));
    } else if (exp->expType == ExpType::LITERAL_STRING) {
        std::cout << "data allocation is not supported yet" << std::endl;
        return constantVariable(-114514);
    }
    auto tmp = newIR<AssignIR>(newVariable(), constantVariable(-114514));
    target.push_back(tmp);
    return tmp->target;
}

void CodeBlock::translateDecAssignment(Node *valueExp, std::shared_ptr<IRVariable> &allocatedVar,
                                       CodeBlockVector &target) {
    // Read the comment in header file
    auto valueVar = translateExp(valueExp, target);
    target.push_back(newIR<StoreAddressIR>(allocatedVar, valueVar));
}

void CodeBlock::translateConditionExp(Node *expRoot, std::shared_ptr<LabelDefIR> &trueLabel,
                                      std::shared_ptr<LabelDefIR> &falseLabel, CodeBlockVector &target) {
    auto exp = expRoot->container->castTo<Exp>();
    if (exp->expType == ExpType::AND) {

    } else if (exp->expType == ExpType::OR) {

    } else {

    }
}

Node *getCompStOrStmt(Node *stmt) {
    assert(stmt->tokenName == "Stmt");
    if (stmt->children[0]->tokenName == "CompSt") return stmt->children[0];
    return stmt;
}

void CodeBlock::translateStmt(Node *stmtNode) {
    // stmtNode may be def or Stmt
    if (stmtNode->tokenName == "Def") {
        // Def -> Specifier DecList SEMI
        auto def = stmtNode->container->castTo<Def>();
        for (const auto &dec: def->declares) {
            auto identifier = dec->identifier.operator*();
            auto allocateIr = newAllocatedVariable(identifier);
            this->content.push_back(allocateIr);
            if (dec->hasInitialValue)
                translateDecAssignment(dec->initialValueExpNode, allocateIr->variable, content);
        }
    } else { // Stmt
        auto &stmtObj = stmtNode->container->castTo<Stmt>().operator*();
        if (stmtObj.stmtType == StmtType::SINGLE) {
            translateExp(stmtNode->children[0], this->content);
        } else if (stmtObj.stmtType == StmtType::RETURN) {
//            auto retIr = newIR<ReturnIR>()
        } else if (stmtObj.stmtType == StmtType::BREAK) {

        } else if (stmtObj.stmtType == StmtType::CONTINUE) {

        } else if (stmtObj.stmtType == StmtType::COMP) {
            auto compBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode),
                                                                shared_from_base<CodeBlock>());
            this->content.push_back(compBlock);
            compBlock->startTranslation();
        } else if (stmtObj.stmtType == StmtType::IF || stmtObj.stmtType == StmtType::IF_ELSE) {
            auto thenLabel = newLabel(LabelType::IF_True, stmtObj.node->lineno);
            auto elseLabel = newLabel(LabelType::IF_False, stmtObj.node->lineno);
            // append IF IR
            translateConditionExp(stmtNode->children[2], thenLabel, elseLabel, this->content);
            auto thenBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode->children[4]),
                                                                shared_from_base<CodeBlock>());
            // append thenLabel (if needed), and thenBlock
//            if (!thenLabel->references.empty())
            this->content.push_back(thenLabel);
            this->content.push_back(thenBlock);
            thenBlock->startTranslation();
            if (stmtObj.stmtType == StmtType::IF) {
                // if-then:
                // IF xx GOTO else_label
                // then block
                // else_label:
                this->content.push_back(elseLabel);
            } else {
                // if-then-else:
                // IF xx GOTO else_label
                // then_label:
                // then block
                // GOTO if_end_label
                // else_label:
                // else_block
                // if_end_label:
                auto endLabel = newLabel(LabelType::IF_END, stmtObj.node->lineno);
                auto gotoEnd = newIR<GotoIR>(endLabel);
                this->content.push_back(gotoEnd);
                this->content.push_back(elseLabel);
                auto elseBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode->children[6]),
                                                                    shared_from_base<CodeBlock>());
                elseBlock->startTranslation();
                this->content.push_back(elseBlock);
                this->content.push_back(endLabel);
            }
        } else if (stmtObj.stmtType == StmtType::FOR) {
            auto forBlock = std::make_shared<ForCodeBlock>(stmtNode, shared_from_base<CodeBlock>());
            this->content.push_back(forBlock);
            forBlock->startTranslation();
        } else if (stmtObj.stmtType == StmtType::WHILE) {
            auto whileBlock = std::make_shared<WhileCodeBlock>(stmtNode, shared_from_base<CodeBlock>());
            this->content.push_back(whileBlock);
            whileBlock->startTranslation();
        } else throw std::runtime_error("unexpected stmt");
    }
}

FunctionCodeBlock::FunctionCodeBlock(Node *extDefNode)
        : CodeBlock(CodeBlockType::Function, extDefNode, nullptr) {
    this->currentScope = rootNode->children[2]->children[0]->container->castTo<Scope>();
    std::string funcName = rootNode->children[1]->children[0]->data;
    auto funDefIr = newIR<FunctionDefIR>(funcName);
    this->content.push_back(funDefIr);
    this->functionDefIr = funDefIr;
    // funDefIr contains function label and params
}

void FunctionCodeBlock::startTranslation() {
    // copy the value of params into allocated stack variable
    for (const auto &item: functionDefIr.lock()->functionType->funcArgs.operator*()) {
        // Special allocation!
        // Allocate space for function params, store the value of param into the address,
        //  and restore the shallow param names: __p_ID (PARAM) -> ID (allocated IRVar)
        //  This keeps the assumption that every variable is allocated in stack.
        // Don't use `newAllocatedVariable` here, which use incorrect size from CType
        //  , but we actually need a pointer to PARAM.
        auto paramVar = std::make_shared<IRVariable>(PARAM_PREFIX + item.first, item.second,
                                                     shared_from_base<CodeBlock>());
        this->variables.push_back(paramVar);
        IRVariablePtr allocVar = std::make_shared<IRVariable>(item.first, IRVariableType::Param,
                                                              shared_from_base<CodeBlock>());
        this->variables.push_back(allocVar);
        this->allocatedVariables[item.first] = allocVar;
        auto allocIr = newIR<AllocateIR>(4, allocVar, allocVar->name);
        this->content.push_back(allocIr);
        auto storeIr = newIR<StoreAddressIR>(allocVar, paramVar);
        this->content.push_back(storeIr);
    }
    Node *compSt = rootNode->children[2];
    assert(compSt->tokenName == "CompSt");
    auto stmts = Node::convertTreeToVector(compSt->children[1], "StmtList", {"Def", "Stmt"});
    for (const auto &stmt: stmts) {
        translateStmt(stmt);
    }
}

ForCodeBlock::ForCodeBlock(Node *stmtNode, const std::shared_ptr<CodeBlock> &parentBlock)
        : CodeBlock(CodeBlockType::For, stmtNode, parentBlock) {
    if (stmtNode->children[0]->container) {
        assert(stmtNode->children[0]->container->getContainerType() == ContainerType::Scope);
        // if the Scope object is bound to FOR node
        this->currentScope = stmtNode->children[0]->container->castTo<Scope>();
    } else {
        auto forBlockStmt = stmtNode->children[8];
        assert(forBlockStmt->children[0]->tokenName == "CompSt");
        assert(forBlockStmt->children[0]->children[0]->tokenName == "LC");
        assert(forBlockStmt->children[0]->children[0]->container);
        assert(forBlockStmt->children[0]->children[0]->container->getContainerType() == ContainerType::Scope);
        this->currentScope = forBlockStmt->children[0]->children[0]->container->castTo<Scope>();
    }
}

void ForCodeBlock::startTranslation() {
    assert(rootNode->tokenName == "Stmt" && rootNode->children[0]->tokenName == "FOR");
    // Stmt -> FOR LP DefOrExp SEMI Exp SEMI MultiExp RP Stmt
    this->loopConditionLabel = newLabel(LabelType::LOOP_CONDITION, rootNode->lineno);
    this->loopBlockLabel = newLabel(LabelType::LOOP_BLOCK, rootNode->children[4]->lineno);
    this->loopEndLabel = newLabel(LabelType::LOOP_END, rootNode->children[8]->lineno);
    this->gotoLoopCondition = newIR<GotoIR>(this->loopConditionLabel);
    // First, Generate loopEntry CodeBlock.
    // DefOrExp -> Specifier DecList | Exp
    auto defOrExp = rootNode->children[2];
    if (defOrExp->children[0]->tokenName == "Exp") {
        // DefOrExp -> Exp
        translateExp(defOrExp->children[0], this->loopEntry);
    } else {
        // DefOrExp -> Specifier DecList
        // allocate for Declares and assign initialValue
        auto def = defOrExp->container->castTo<Def>();
        for (const auto &dec: def->declares) {
            auto allocateIr = newAllocatedVariable(dec->identifier.operator*());
            this->loopEntry.push_back(allocateIr);
            if (dec->hasInitialValue)
                translateDecAssignment(dec->initialValueExpNode, allocateIr->variable, this->loopEntry);
        }
    }
    // Second, Generate Loop_Condition CodeBlock
    auto conditionalExp = rootNode->children[4];
    // trueLabel -> loopBlockLabel, falseLabel -> loopEndLabel
    translateConditionExp(conditionalExp, this->loopBlockLabel, this->loopEndLabel, this->loopCondition);
    // Third, Generate Loop_Next CodeBlock
    auto multiExp = rootNode->children[6];
    auto exps = Node::convertTreeToVector(multiExp, "MultiExp", {"Exp"});
    for (const auto &exp: exps)
        translateExp(exp, this->loopNext);
    // Forth, Generate For Statements
    auto stmt = rootNode->children[8];
    translateStmt(stmt);
}

void ForCodeBlock::generateIr(std::ostream &ostream) {
    for (const auto &item: this->loopEntry) item->generateIr(ostream);
    this->loopConditionLabel->generateIr(ostream);
    for (const auto &item: this->loopCondition) item->generateIr(ostream);
    this->loopBlockLabel->generateIr(ostream);
    for (const auto &item: this->content) item->generateIr(ostream);
    ostream << "; For Next Expressions" << std::endl;
    for (const auto &item: this->loopNext) item->generateIr(ostream);
    this->gotoLoopCondition->generateIr(ostream);
    this->loopEndLabel->generateIr(ostream);
}

WhileCodeBlock::WhileCodeBlock(Node *stmtNode, const std::shared_ptr<CodeBlock> &parentBlock)
        : CodeBlock(CodeBlockType::While, stmtNode, parentBlock) {

}

void WhileCodeBlock::startTranslation() {

}

void WhileCodeBlock::generateIr(std::ostream &ostream) {

}

GeneralCodeBlock::GeneralCodeBlock(Node *rootNode, const std::shared_ptr<CodeBlock> &parentBlock)
        : CodeBlock(CodeBlockType::General, rootNode, parentBlock) {
    if (rootNode->tokenName == "CompSt") {
        this->currentScope = rootNode->children[0]->container->castTo<Scope>();
    } else if (rootNode->tokenName == "StmtList") {
        this->currentScope = parentBlock->currentScope;
    } else if (rootNode->tokenName == "Stmt") {
        this->currentScope = parentBlock->currentScope;
    }
}

void GeneralCodeBlock::startTranslation() {
    if (rootNode->tokenName == "Stmt") {
        translateStmt(rootNode);
    } else if (rootNode->tokenName == "StmtList") {
        auto stmts = Node::convertTreeToVector(rootNode, "StmtList", {"Stmt"});
        for (const auto &stmt: stmts)
            translateStmt(stmt);
    } else if (rootNode->tokenName == "CompSt") {
        auto stmts = Node::convertTreeToVector(rootNode->children[1], "StmtList", {"Stmt"});
        for (const auto &stmt: stmts)
            translateStmt(stmt);
    }
}

void GeneralCodeBlock::generateIr(std::ostream &ostream) {
    ostream << "\t; TODO" << std::endl;
}
