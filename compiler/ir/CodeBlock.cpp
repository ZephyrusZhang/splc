#include <sstream>
#include "CodeBlock.h"
#include "../Node.h"

CodeBlock::CodeBlock(Node *rootNode) : rootNode(rootNode) {
    if (rootNode->tokenName == "ExtDef" && rootNode->children[1]->tokenName == "FunDec") {
        this->CodeBlockType = CodeBlockType::Function;
        std::string funcName = rootNode->children[1]->children[0]->data;
        auto funDefIR = std::make_shared<FunctionDefIR>(funcName);
        this->content.push_back(funDefIR);
        translateFunction();
    }
}

std::shared_ptr<IRVariable> CodeBlock::newVariable(IRVariableType expectedType) {
    size_t cnt = this->variableCounts[expectedType]++;
    std::ostringstream name;
    if (expectedType == IRVariableType::Int) name << 'i';
    else if (expectedType == IRVariableType::ArrayIndex) name << 'x';
    else if (expectedType == IRVariableType::BaseAddress) name << 'b';
    else if (expectedType == IRVariableType::StructOffset) name << 'o';
    else name << "u";
    name << cnt;
    auto var = std::make_shared<IRVariable>(expectedType, name.str(), this->weak_from_this());
    this->variables.push_back(var);
    return var;
}

void CodeBlock::translateFunction() {
    Node * compSt = rootNode->children[2];
    assert(compSt->tokenName == "CompSt");
    auto stmts = Node::convertTreeToVector(compSt->children[1], "StmtList", {"Def", "Stmt"});
    for (const auto &stmt: stmts) {
        if (stmt->tokenName == "Def") {

        } else {

        }
    }
}