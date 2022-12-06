#ifndef SPLC_CODEBLOCK_H
#define SPLC_CODEBLOCK_H
#include <vector>
#include <memory>
#include <map>
#include "../Scope.h"
#include "IR.h"

enum CodeBlockType {
    Function,
    For,
    While,
    If,
    General
};

class CodeBlock : std::enable_shared_from_this<CodeBlock> {
private:
    const Node *rootNode;
    std::shared_ptr<Scope> currentScope;
    std::map<IRVariableType, size_t> variableCounts;
public:
    CodeBlockType CodeBlockType;
    std::vector<std::shared_ptr<IRVariable>> variables;

    std::vector<std::shared_ptr<IR>> content;
    std::vector<std::shared_ptr<IR>> endBlock;

private:
    std::shared_ptr<IRVariable> newVariable(IRVariableType expectedType);
    void translateFunction();
public:
    explicit CodeBlock(Node* rootNode);

};

#endif //SPLC_CODEBLOCK_H
