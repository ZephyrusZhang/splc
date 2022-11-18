#ifndef SPLC_COMPOUNDTYPE_H
#define SPLC_COMPOUNDTYPE_H

#include <memory>
#include "Specifier.h"
#include "Dec.h"

class CompoundType {
private:
public:
    CompoundType() = default;
    explicit CompoundType(BasicType type);

    BasicType type = TypeUnknown;
    // for pointer, Array should be converted into pointer to PrimitiveTypes
    std::shared_ptr<CompoundType> pointTo;
    int maxIndex = 0;
    // for struct:
    typedef std::pair<std::string, CompoundType> StructDefList;
    std::shared_ptr<std::vector<StructDefList>> structDefLists;
    // for function definition:
    std::shared_ptr<std::vector<CompoundType>> funcArgs;

    explicit CompoundType(const Specifier& specifier);
    explicit CompoundType(const Specifier& specifier, const Dec& dec);

    CompoundType(const CompoundType& compoundType) = default;
    CompoundType(CompoundType&& compoundType) = default;
    CompoundType& operator=(const CompoundType& compoundType) = delete;
    CompoundType& operator=(CompoundType&& compoundType) = default;

    friend std::ostream& operator<<(std::ostream& os, const CompoundType& type);

    // Type Checking
    friend bool operator==(const CompoundType& o1, const CompoundType& o2);
    bool canDoArithmetic() const;
    bool canDoBoolean() const;
    bool canCompare() const;

    static bool canAssignment(const CompoundType& left, const CompoundType& right);
};

#endif //SPLC_COMPOUNDTYPE_H
