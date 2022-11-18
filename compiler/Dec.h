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
    bool hasInitialValue = false;

    // Function Dec:
    typedef std::pair<std::shared_ptr<Specifier>, std::shared_ptr<Dec>> ParmaDec;
    std::unique_ptr<std::vector<ParmaDec>> funcDec;

    [[nodiscard]] bool isArray() const noexcept;
    void installChild(const std::vector<Node *>& children) override;

    friend std::ostream& operator<<(std::ostream& os, const Dec& dec);
};


#endif //SPLC_DEC_H
