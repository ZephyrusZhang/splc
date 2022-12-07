#ifndef SPLC_CODEBLOCK_H
#define SPLC_CODEBLOCK_H
#include <vector>
#include <memory>
#include <map>
#include "Scope.h"
#include "IR.h"

enum class CodeBlockType {
    Function,
    For,
    While,
    General
};

enum class LabelType {
    IF_False,
    IF_True,
    IF_END,
};

class CodeBlock : public IR {
protected:
    const Node *rootNode;
    const std::weak_ptr<CodeBlock> parentBlock;
private:
    std::map<IRVariableType, size_t> variableCounts;
    std::map<LabelType, size_t> labelCounts;
public:
    const CodeBlockType codeBlockType;
    std::shared_ptr<Scope> currentScope;
    std::vector<std::shared_ptr<IRVariable>> variables;

    std::vector<std::shared_ptr<IR>> content;
    std::vector<std::shared_ptr<CodeBlock>> nextBlocks;

protected:
    std::shared_ptr<IRVariable> newVariable(IRVariableType expectedType);
    std::shared_ptr<LabelDefIR> newLabel(LabelType type, int lineno);
    void translateExp(Node * expRoot);
    void translateStmt(Node * expRoot); // return false should stop generate from vector<Node * Stmt>
    void translateFunction();
public:
    explicit CodeBlock(CodeBlockType type, Node *rootNode, const std::shared_ptr<CodeBlock>& parentBlock);
    virtual void startTranslation() = 0;

    void generateIr(std::ostream &ostream) override;

};

class FunctionCodeBlock : public CodeBlock {
public:
    explicit FunctionCodeBlock(Node* extDefNode);
    void startTranslation() override;
};

class ForCodeBlock : public CodeBlock {
public:
    explicit ForCodeBlock(Node* stmtNode, std::shared_ptr<CodeBlock> parentBlock);
    void startTranslation() override;
    void generateIr(std::ostream &ostream) override;
};

class WhileCodeBlock : public CodeBlock {
public:
    explicit WhileCodeBlock(Node* stmtNode, std::shared_ptr<CodeBlock> parentBlock);
    void startTranslation() override;
    void generateIr(std::ostream &ostream) override;
};

class GeneralCodeBlock : public CodeBlock {
public:
    explicit GeneralCodeBlock(Node* StmtListNode, const std::shared_ptr<CodeBlock>& parentBlock);
    void startTranslation() override;

    void generateIr(std::ostream &ostream) override;
};

#endif //SPLC_CODEBLOCK_H
