#include "CompoundType.h"
#include "Specifier.h"
#include "Dec.h"
#include "Def.h"
#include "Scope.h"

CompoundType::CompoundType(BasicType type) {
    this->type = type;
}


CompoundType::CompoundType(const Specifier &specifier, const Dec &dec) : CompoundType(specifier) {
    if (dec.isArray()) {
        CompoundType *current = this;
        for (size_t i = 0; i < dec.arraySize->size(); i++) {
            const auto &item = dec.arraySize.operator*()[i];
            current->maxIndex = item;
            current->type = TypePointer;
            current->pointTo = std::make_shared<CompoundType>(specifier);
            if (i == dec.arraySize->size() - 1) {
                // the last level ptr, should be specifier's type
                current->pointTo->type = specifier.type;
            } else
                current->pointTo->type = TypePointer;
            current = current->pointTo.get();
        }
    } else if (dec.funcDec) {
        // Function Args
        this->funcArgs = std::make_unique<std::vector<CompoundType>>();
        for (const auto &item: dec.funcDec.operator*()) {
            this->funcArgs->emplace_back(CompoundType(*item.first, *item.second));
        }
    }
}

CompoundType::CompoundType(const Specifier &specifier) {
    // this must not be an array
    this->type = specifier.type;
    if (this->type == TypePointer) {
        this->pointTo = std::make_unique<CompoundType>(*specifier.pointTo);
    } else if (this->type == TypeStruct) {
        if (specifier.structDefList) {
            this->structDefLists = std::make_shared<std::vector<StructDefList>>();
            for (const auto &def: specifier.structDefList.operator*()) {
                for (const auto &dec: def->declares) {
                    CompoundType itemType(*def->specifier, *dec);
                    this->structDefLists->emplace_back(*dec->identifier, std::move(itemType));
                }
            }
        } else {
            // lookup SymbolTable to find Struct definition.
            const auto& predefined = Scope::getCurrentScope()->lookupSymbol(specifier.structName);
            this->structDefLists = predefined->structDefLists;
        }
    }
}

std::ostream &operator<<(std::ostream &os, const CompoundType &type) {
    switch (type.type) {
        case TypeUnknown: os << "unknown"; break;
        case TypeVoid:  os << "void"; break;
        case TypeChar: os << "char"; break;
        case TypeInt: os << "int"; break;
        case TypeFloat: os << "float"; break;
        case TypeStruct: os << "struct"; break;
        case TypePointer: os << "ptrTo"; break;
    }
    if (type.type == TypePointer) {
        if (type.maxIndex > 0) os << "[" << type.maxIndex << "]";
        os << ":" << const_cast<const CompoundType &>(*type.pointTo);
    } else if (type.type == TypeStruct) {
        os << ": {";
        for (const auto &item: type.structDefLists.operator*()) {
            os << item.first << ":" << item.second << "; ";
        }
        os << "}";
    }
    if (type.funcArgs) {
        os << " function(";
        for (int i = 0; i < type.funcArgs->size(); i++) {
            os << const_cast<const CompoundType&>(type.funcArgs.operator*()[i]);
            if (i != type.funcArgs->size() - 1) os << ", ";
        }
        os << ")";
    }
    return os;
}

bool operator==(const CompoundType &o1, const CompoundType &o2) {
    if (o1.type == BasicType::TypeUnknown || o2.type == BasicType::TypeUnknown) return false;
    if (o1.type != o2.type) return false;
    if (o1.type == TypeStruct) return *o1.structDefLists == *o2.structDefLists;
    if (o1.type == TypePointer) return *o1.pointTo == *o2.pointTo;
    return o1.type == o2.type;
}

bool CompoundType::canDoArithmetic() const {
    return true;
}

bool CompoundType::canDoBoolean() const {
    return true;
}

bool CompoundType::canCompare() const {
    return false;
}
