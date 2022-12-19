#ifndef SPLC_COMPOUNDTYPE_H
#define SPLC_COMPOUNDTYPE_H

#include <memory>
#include "Specifier.h"
#include "Dec.h"

class CompoundType {
public:
    CompoundType() = default;
    explicit CompoundType(BasicType type);

    BasicType type = TypeUnknown;
    // for pointer, Array should be converted into pointer to PrimitiveTypes
    std::shared_ptr<CompoundType> pointTo;
    int maxIndex = 0;
    // for struct:
    typedef std::pair<std::string, CompoundType> StructDefList;
private:
    std::shared_ptr<std::vector<StructDefList>> structDefLists;
public:
    [[nodiscard]] std::shared_ptr<std::vector<StructDefList>> getStructDefLists();
    [[nodiscard]] std::shared_ptr<const std::vector<StructDefList>> getStructDefLists() const;
    std::shared_ptr<std::string> unresolvedStructName;
    // for function definition:
    typedef std::pair<std::string, CompoundType> FuncArg;
    std::shared_ptr<std::vector<FuncArg>> funcArgs;

    explicit CompoundType(const Specifier& specifier);
    explicit CompoundType(const Specifier& specifier, const Dec& dec);

    CompoundType(const CompoundType& compoundType) = default;
    CompoundType(CompoundType&& compoundType) = default;
    CompoundType& operator=(const CompoundType& compoundType) = delete;
    CompoundType& operator=(CompoundType&& compoundType) = default;

    friend std::ostream& operator<<(std::ostream& os, const CompoundType& type);

    // Type Checking
    friend bool operator==(const CompoundType& o1, const CompoundType& o2);
    [[nodiscard]] bool canDoArithmetic() const;
    [[nodiscard]] bool canDoBoolean() const;
    [[nodiscard]] bool canCompare() const;

    [[nodiscard]] int32_t sizeOf() const;
    [[nodiscard]] bool isArray() const {
        return type == TypePointer && maxIndex > 0;
    }
    [[nodiscard]] int32_t getStructOffset(const std::string& id) const {
        assert(this->type == TypeStruct);
        assert(this->structDefLists);
        int32_t offset = 0;
        for (const auto &item: structDefLists.operator*()) {
            if (item.first == id) return offset;
            offset += item.second.sizeOf();
        }
        return offset;
    }

    static bool canAssignment(const CompoundType& left, const CompoundType& right);

};

#endif //SPLC_COMPOUNDTYPE_H
