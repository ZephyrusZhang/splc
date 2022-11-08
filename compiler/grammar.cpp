#include <iostream>
#include <vector>
#include "grammar.h"
#include "type.h"

std::shared_ptr<Container> Container::generateContainer(const std::string &tokenName, DataType type, const std::string &data) {
    if (tokenName == "Specifier") {
        std::cout << "meet Specifier " << tokenName << std::endl;
        return std::make_shared<Specifier>(tokenName, BasicType::TypeUnknown);
    }
    return nullptr;
}

void Scope::installChild(std::vector<Node *>) {

}
