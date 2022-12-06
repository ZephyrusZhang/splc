#include <sstream>
#include "CodeBlock.h"
#include "Node.h"
#include "Def.h"
#include "Stmt.h"

CodeBlock::CodeBlock(CodeBlockType type, Node *rootNode, std::shared_ptr<CodeBlock> parentBlock)
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
    return std::make_shared<LabelDefIR>(ss.str());
}

void CodeBlock::generateIr(std::ostream &ostream) {
    for (const auto &item: this->content)
        item->generateIr(ostream);
    for (const auto &item: this->nextBlocks)
        item->generateIr(ostream);
}

void CodeBlock::translateExp(Node *expRoot) {

}

Node * getCompStOrStmt(Node* stmt) {
    assert(stmt->tokenName == "Stmt");
    if (stmt->children[0]->tokenName == "CompSt") return stmt->children[0];
    return stmt;
}

bool CodeBlock::translateStmt(Node* stmtNode) {
    // stmtNode may be def or Stmt
    if (stmtNode->tokenName == "Def") {
        auto def = stmtNode->container->castTo<Def>();
        for (const auto &item: def->declares) {
            auto identifier = item->identifier.operator*();
            auto cType = this->currentScope->lookupSymbol(identifier).operator*();
            auto irVar = std::make_shared<IRVariable>(identifier, cType, shared_from_base<CodeBlock>());
            auto allocateIr = std::make_shared<AllocateIR>(cType.sizeOf(), irVar, identifier);
            this->variables.push_back(irVar);
            this->content.push_back(allocateIr);
        }
        return true;
    } else { // Stmt
        auto& stmtObj = stmtNode->container->castTo<Stmt>().operator*();
        if (stmtObj.stmtType == StmtType::SINGLE) {
            translateExp(stmtNode->children[0]);
        } else if (stmtObj.stmtType == StmtType::RETURN) {

        } else if (stmtObj.stmtType == StmtType::BREAK) {

        } else if (stmtObj.stmtType == StmtType::CONTINUE) {

        } else if (stmtObj.stmtType == StmtType::COMP) {
            auto compBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode), shared_from_base<CodeBlock>());
            this->content.push_back(compBlock);
            compBlock->startTranslation();
            return false;
        } else if (stmtObj.stmtType == StmtType::IF || stmtObj.stmtType == StmtType::IF_ELSE) {
            // TODO: Analyze if statement condition
            auto condition = newVariable(IRVariableType::Int);
            this->content.push_back(std::make_shared<AssignIR>(condition, std::make_shared<IRConstant>("1")));

            auto elseLabel = newLabel(LabelType::IF_False, stmtObj.node->lineno);
            auto irIf = std::make_shared<IfIR>(condition, elseLabel);
            auto thenBlock = std::make_shared<GeneralCodeBlock>(getCompStOrStmt(stmtNode->children[4]), shared_from_base<CodeBlock>());
            // append IF IR
            this->content.push_back(irIf);
            // append thenBlock
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

        } else if (stmtObj.stmtType == StmtType::WHILE) {

        }
        return true;
    }
}

FunctionCodeBlock::FunctionCodeBlock(Node *ExtDefNode)
        : CodeBlock(CodeBlockType::Function, ExtDefNode, nullptr) {
    this->currentScope = rootNode->children[2]->children[0]->container->castTo<Scope>();
    std::string funcName = rootNode->children[1]->children[0]->data;
    auto funDefIR = std::make_shared<FunctionDefIR>(funcName);
    this->content.push_back(funDefIR);
    for (const auto &item: funDefIR->functionType->funcArgs.operator*()) {
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

ForCodeBlock::ForCodeBlock(Node *StmtNode, std::shared_ptr<CodeBlock> parentBlock)
        : CodeBlock(CodeBlockType::For, StmtNode, parentBlock) {

}

void ForCodeBlock::startTranslation() {

}

void ForCodeBlock::generateIr(std::ostream &ostream) {

}

WhileCodeBlock::WhileCodeBlock(Node *StmtNode, std::shared_ptr<CodeBlock> parentBlock)
        : CodeBlock(CodeBlockType::While, StmtNode, parentBlock) {

}

void WhileCodeBlock::startTranslation() {

}

void WhileCodeBlock::generateIr(std::ostream &ostream) {

}

GeneralCodeBlock::GeneralCodeBlock(Node *rootNode, std::shared_ptr<CodeBlock> parentBlock)
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

}

void GeneralCodeBlock::generateIr(std::ostream &ostream) {
    ostream << "\t; TODO" << std::endl;
}
