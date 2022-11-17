#ifndef SPLC_DEC_H
#define SPLC_DEC_H
#include "Specifier.h"
#include "Container.h"

// class Dec can be Dec or VarDec
class Dec : public Container {
public:
    const static ContainerType containerType = ContainerType::Dec;
    explicit Dec(Node* pNode) : Container(pNode, containerType) {

    }
    ~Dec() override = default;

    std::unique_ptr<const std::string> identifier;
    std::unique_ptr<std::vector<int>> arraySize;

    // Only available when Dec -> VarDec ASSIGN Exp
    std::unique_ptr<Node> initialValue;

    bool isArray() const noexcept;
    void installChild(const std::vector<Node *>& children) override;
    void installToSymbolTable(std::shared_ptr<Specifier>& specifier);

    friend std::ostream& operator<<(std::ostream& os, const Dec& dec);
};


#endif //SPLC_DEC_H