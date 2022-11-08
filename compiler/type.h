#ifndef SPLC_TYPE
#define SPLC_TYPE
#include <string>
#include <utility>
#include <memory>

enum BasicType {
    TypeVoid,
    TypeChar,
    TypeInt,
    TypeFloat,
    TypeStruct,
    TypePointer
};

class Type {
public:
    const std::string name;
    const BasicType type;
    std::string structName;
    std::unique_ptr<Type> pointTo;

    Type(std::string name, const BasicType type) : name(std::move(name)), type(type) {
        if (type == BasicType::TypeStruct) {
            structName = "test";
        } else if (type == BasicType::TypePointer) {

        }
    }
};
#endif