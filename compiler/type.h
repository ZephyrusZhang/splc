#include <string>
#include <utility>

enum BasicType {
    TypeVoid,
    TypeChar,
    TypeInt,
    TypeFloat,
    TypeStruct
};

class Type
{
public:
    const std::string name;
    const BasicType type;
    Type(std::string  name, const BasicType type): name(std::move(name)), type(type) {
        if (type == BasicType::TypeStruct) {
            
        }
    }
};