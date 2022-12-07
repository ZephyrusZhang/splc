#include <sstream>
#include <utility>
#include "CodeBlock.h"
#include "Node.h"
#include "Def.h"
#include "Stmt.h"

CodeBlock::CodeBlock(CodeBlockType type, Node *rootNode, const std::shared_ptr<CodeBlock>& parentBlock)
        : IR(IRType::CodeBlock), codeBlockType(type), rootNode(rootNode), parentBlock(parentBlock) {}

std::shared_ptr<IRVariable> CodeBlock::newVariable(IRVariableType expectedType) {
    size_t cnt = this->variableCounts[expectedType]++;
    std::ostringstream name;
    if (expectedType == IRVariableType::Int) name << 'i';
    else if (expectedType == IRVariableType::ArrayIndex) name << 'x';
    else if (expectedType == IRVariableType::Pointer) name << "p";
    else if (expectedType == IRVariableType::BaseAddress) name << 'b';
    else if (expectedType == IRVariableType::StructOffset) name << 'o';
    else name << "u";
    name << cnt;
    auto var = std::make_shared<IRVariable>(expectedType, name.str(), shared_from_base<CodeBlock>());
    this->variables.push_back(var);
    return var;
}

std::shared_ptr<LabelDefIR> CodeBlock::newLabel(LabelType type, int lineno) {
    std::stringstream ss;
    if (type == LabelType::IF_False) ss << "if_F_" ;
    else if (type == LabelType::IF_True) ss << "if_T_" ;
    else if (type == LabelType::IF_END) ss << "if_E_" ;
    ss << this->labelCounts[type]++ << "_" << lineno;
    auto label = std::make_shared<LabelDefIR>(ss.str());
    this->labels.push_back(label);
    return label;
}

std::shared_ptr<AllocateIR> CodeBlock::newAllocatedVariable(std::string identifier) {
    if (this->allocatedVariables.find(identifier) != this->allocatedVariables.end()) {
        throw std::runtime_error("identifier is already allocated");
    }
    auto cType = this->currentScope->lookupSymbol(identifier).operator*();
    auto irVar = std::make_shared<IRVariable>(identifier, cType, shared_from_base<CodeBlock>());
    auto allocateIr = std::make_shared<AllocateIR>(cType.sizeOf(), irVar, identifier);
    this->variables.push_back(irVar);
    this->allocatedVariables[identifier] = irVar;
    return allocateIr;
}

std::shared_ptr<IRVariable> CodeBlock::getAllocatedVariable(const std::string& identifier) {
    if (this->allocatedVariables.find(identifier) == this->allocatedVariables.end()) {
        throw std::runtime_error("unallocated identifier when trying to get IRVariable");
    } else {
        return this->allocatedVariables[identifier];
    }
}

void CodeBlock::generateIr(std::ostream &ostream) {
    for (const auto &item: this->content)
        item->generateIr(ostream);
}

void CodeBlock::translateExp(Node *expRoot, CodeBlockVector& target) {
    return {};
}

void CodeBlock::translateDecAssignment(Node *valueExp, std::shared_ptr<IRVariable> &assignTo,
                                       CodeBlockVector &target) {
    // Read the comment in header file
}

void CodeBlock::translateConditionExp(Node *expRoot, std::shared_ptr<LabelDefIR> &trueLabel, std::shared_ptr<LabelDefIR> &falseLabel, CodeBlockVector &target) {

}

Node * getCompStOrStmt(Node* stmt) {
    assert(stmt->tokenName == "Stmt");
    if (stmt->children[0]->tokenName == "CompSt") return stmt->children[0];
    return stmt;
}

void CodeBlock::translateStmt(Node* stmtNode) {
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
        auto& stmtObj = stmtNode->container->castTo<Stmt>().operator*();
        if (stmtObj.stmtType == StmtType::SINGLE) {
            translateExp(stmtNode->children[0], this->content);
        } else if (stmtObj.stmtType == StmtType::RETURN) {

        } else if (stmtObj.stmtType == StmtType::BREAK) {

        } else if (stmtObj.stmtType == StmtType::CONTINUE) {

        } else if (stmtObj.stmtType == StmtType::COMP) {
            auto compBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode), shared_from_base<CodeBlock>());
            this->content.push_back(compBlock);
            compBlock->startTranslation();
        } else if (stmtObj.stmtType == StmtType::IF || stmtObj.stmtType == StmtType::IF_ELSE) {
            auto thenLabel = newLabel(LabelType::IF_True, stmtObj.node->lineno);
            auto elseLabel = newLabel(LabelType::IF_False, stmtObj.node->lineno);
            // append IF IR
            translateConditionExp(stmtNode->children[2], thenLabel, elseLabel, this->content);
            auto thenBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode->children[4]), shared_from_base<CodeBlock>());
            // append thenLabel (if need), and thenBlock
            if (!thenLabel->references.empty())
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
                auto gotoEnd = std::make_shared<GotoIR>(endLabel);
                this->content.push_back(gotoEnd);
                this->content.push_back(elseLabel);
                auto elseBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode->children[6]), shared_from_base<CodeBlock>());
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
    auto funDefIr = std::make_shared<FunctionDefIR>(funcName);
    this->content.push_back(funDefIr);
    for (const auto &item: funDefIr->functionType->funcArgs.operator*()) {
        auto irVar = std::make_shared<IRVariable>(item.first, item.second, shared_from_base<CodeBlock>());
        this->variables.push_back(irVar);
    }
}

void FunctionCodeBlock::startTranslation() {
    Node *compSt = rootNode->children[2];
    assert(compSt->tokenName == "CompSt");
    auto stmts = Node::convertTreeToVector(compSt->children[1], "StmtList", {"Def", "Stmt"});
    for (const auto &stmt: stmts) {
        translateStmt(stmt);
    }
}

ForCodeBlock::ForCodeBlock(Node *stmtNode, const std::shared_ptr<CodeBlock>& parentBlock)
        : CodeBlock(CodeBlockType::For, stmtNode, parentBlock) {
    if (stmtNode->children[0]->container && stmtNode->children[0]->container->getContainerType() == ContainerType::Scope) {
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

}

WhileCodeBlock::WhileCodeBlock(Node *stmtNode, const std::shared_ptr<CodeBlock>& parentBlock)
        : CodeBlock(CodeBlockType::While, stmtNode, parentBlock) {

}

void WhileCodeBlock::startTranslation() {

}

void WhileCodeBlock::generateIr(std::ostream &ostream) {

}

GeneralCodeBlock::GeneralCodeBlock(Node *rootNode, const std::shared_ptr<CodeBlock>& parentBlock)
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
