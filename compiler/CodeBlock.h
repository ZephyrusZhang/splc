#ifndef SPLC_CODEBLOCK_H
#define SPLC_CODEBLOCK_H
#include <vector>
#include <memory>
#include <map>
#include "Scope.h"
#include "IR.h"
#include "Exp.h"

enum class CodeBlockType {
    Function,
    For,
    While,
    General
};

enum class LabelType {
    IF_False = 0,
    IF_True,
    IF_END,
    LOOP_CONDITION,
    LOOP_END,
    LOOP_BLOCK,
    IF_COND_SHORTCUT,
};
typedef std::vector<std::shared_ptr<IR>> CodeBlockVector;

class CodeBlock : public IR {
protected:
    Node *rootNode;
    const std::weak_ptr<CodeBlock> parentBlock;
private:
    std::map<IRVariableType, size_t> variableCounts;
    std::map<LabelType, size_t> labelCounts;
public:
    const CodeBlockType codeBlockType;
    // Current Scope for this CodeBlock
    std::shared_ptr<Scope> currentScope;
    // Used variables
    std::vector<std::shared_ptr<IRVariable>> variables;
    // Used labels
    std::vector<std::shared_ptr<LabelDefIR>> labels;
    // scope identifier -> allocated variable
    std::map<std::string, std::shared_ptr<IRVariable>> allocatedVariables;
    std::map<std::string, std::shared_ptr<IRVariable>> allocatedVariablesAddress;

    CodeBlockVector content;

protected:
    // Generate temporary variable
    std::shared_ptr<IRVariable> newVariable(IRVariableType expectedType);
    // Generate a constant variable
    std::shared_ptr<IRVariable> constantVariable(int32_t value);
    // Generate a label
    std::shared_ptr<LabelDefIR> newLabel(LabelType type, int lineno);
    // Generate Allocated variable
    std::shared_ptr<AllocateIR> newAllocatedVariable(const std::string& identifier);

    template<typename T, typename... Args>
    std::shared_ptr<T> newIR(Args&&... args) {
        static_assert(std::is_base_of<IR, T>::value, "T should inherit from IR");
        auto ptr = std::make_shared<T>(std::forward<Args>(args)...);
        ptr->countReference();
        return ptr;
    }
    // These three methods shouldn't modify any CodeBlockVector

    // get Allocated Variable by name in syntax
    std::shared_ptr<IRVariable> getAllocatedVariable(const std::string& identifier);
    std::shared_ptr<IRVariable> getAllocatedAddressVariable(const std::string& identifier);

    // Translate given Exp into IRs and append to target, return the IRVariable holds the value
    std::shared_ptr<IRVariable> translateExp(Node *expRoot, CodeBlockVector &target);

    // Translate a LValue Expression, return a IRVar which holds the address of the lvalue.
    std::shared_ptr<IRVariable> translateAddressExp(std::shared_ptr<Exp> exp, CodeBlockVector &target);

    std::shared_ptr<IRVariable>
    translateLogicalExp(Node *expRoot, CodeBlockVector &target, std::shared_ptr<LabelDefIR> shortCutLabel);

    // Translate an Assign Expression, used in variable declared with initial value.
    // Note that the IrVariable assignTo is an address, usually a pointer. Since it is allocated in stack, the value of IrVariable is the allocated start address
    // Example: int a = 123; => translateAssignmentExp(root of 123, a, blocks);
    void translateDecAssignment(Node * valueExp, std::shared_ptr<IRVariable>& allocatedVar, CodeBlockVector& target);

    // Translate a Conditional Expression into IRs, need to consider shortcut !
    // Example: if (aaa && bbb) =>
    //      ia := ???
    //      IF NOT ia GOTO False_Label
    //      ib := ???
    //      IF NOT ib GOTO False_Label
    //    don't need to GOTO True_Label
    void translateConditionExp(Node *expRoot, const std::shared_ptr<LabelDefIR>& trueLabel, const std::shared_ptr<LabelDefIR>& falseLabel, CodeBlockVector &target, bool inverse = true, bool ignoreGoto = false);

    // Translate given Stmt into IRs and append to this->content
    void translateStmt(Node * stmtNode);
public:
    explicit CodeBlock(CodeBlockType type, Node *rootNode, const std::shared_ptr<CodeBlock>& parentBlock);
    // start to translate this block
    virtual void startTranslation() = 0;
    // write generated IRs into ostream
    void generateIr(std::ostream &ostream) override;

    template<typename T>
    bool instanceOf() {
        static_assert(std::is_base_of<CodeBlock, T>::value, "T should inherit from CodeBlock");
        const CodeBlockType classType = T::myCodeBlockType;
        if (this->codeBlockType == classType) return true;
        return false;
    }

    template<typename T>
    std::shared_ptr<T> castCodeBlockTo() {
        static_assert(std::is_base_of<CodeBlock, T>::value, "T should inherit from CodeBlock");
        const CodeBlockType classType = T::myCodeBlockType;
        assert(this->codeBlockType == classType);
        return shared_from_this();
    }
};

class FunctionCodeBlock : public CodeBlock {
private:
    std::weak_ptr<FunctionDefIR> functionDefIr;
public:
    static const CodeBlockType myCodeBlockType = CodeBlockType::Function;
public:
    explicit FunctionCodeBlock(Node* extDefNode);
    void startTranslation() override;
};

class ForCodeBlock : public CodeBlock {
public:
    static const CodeBlockType myCodeBlockType = CodeBlockType::For;
private:
    std::vector<std::shared_ptr<IR>> loopEntry;
    std::vector<std::shared_ptr<IR>> loopCondition;
    std::vector<std::shared_ptr<IR>> loopNext;
    std::shared_ptr<LabelDefIR> loopConditionLabel;
    std::shared_ptr<LabelDefIR> loopBlockLabel;
    std::shared_ptr<LabelDefIR> loopEndLabel;
    std::shared_ptr<GotoIR> gotoLoopCondition;
    // structure:
    // <Loop_Entry>     ; DefOrExp
    // Loop_Condition:  ; label
    // <Loop_Condition> ; Exp, check enter loop or exit
    // Loop_Block:      ; label
    // <Loop_Body>      ; Stmt, CodeBlock::content
    // <Loop_Next>      ; MultiExp
    // GOTO Loop_Condition
    // Loop_End:        ; label
public:
    explicit ForCodeBlock(Node* stmtNode, const std::shared_ptr<CodeBlock>& parentBlock);
    void startTranslation() override;
    void generateIr(std::ostream &ostream) override;
};

class WhileCodeBlock : public CodeBlock {
public:
    static const CodeBlockType myCodeBlockType = CodeBlockType::While;
private:
    // structure:
    // Loop_Condition:  ; label
    // <Loop_Condition> ; Exp, check enter loop or exit
    // Loop_Block:      ; label
    // <Loop_Body>      ; Stmt, CodeBlock::content
    // GOTO Loop_Condition
    // Loop_End:        ; label
    std::vector<std::shared_ptr<IR>> loopCondition;
    std::shared_ptr<LabelDefIR> loopConditionLabel;
    std::shared_ptr<LabelDefIR> loopBlockLabel;
    std::shared_ptr<LabelDefIR> loopEndLabel;
    std::shared_ptr<GotoIR> gotoLoopCondition;
public:
    explicit WhileCodeBlock(Node* stmtNode, const std::shared_ptr<CodeBlock>& parentBlock);
    void startTranslation() override;
    void generateIr(std::ostream &ostream) override;
};

class GeneralCodeBlock : public CodeBlock {
public:
    explicit GeneralCodeBlock(Node* StmtListNode, const std::shared_ptr<CodeBlock>& parentBlock);
    void startTranslation() override;
};

#endif //SPLC_CODEBLOCK_H
