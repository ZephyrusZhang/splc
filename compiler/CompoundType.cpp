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
        this->funcArgs = std::make_shared<std::vector<FuncArg>>();
        for (const auto &item: dec.funcDec.operator*()) {
            this->funcArgs->emplace_back(*item.second->identifier, CompoundType(*item.first, *item.second));
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
                    this->structDefLists->emplace_back(*dec->identifier, CompoundType(*def->specifier, *dec));
                }
            }
        } else {
            // lookup SymbolTable to find struct definition.
            if (!Scope::getCurrentScope()->isSymbolExistsRecursively(specifier.structName)) {
                this->unresolvedStructName = std::make_unique<std::string>(specifier.structName);
                std::cout << "unable to resolve struct " << *this->unresolvedStructName << " in " << this << std::endl;
            } else {
                const auto &predefined = Scope::getCurrentScope()->lookupSymbol(specifier.structName);
                this->structDefLists = predefined->structDefLists;
            }
        }
    }
}

void print(std::ostream& os, const CompoundType& type, int depth) {
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
        os << ":";
        print(os,  const_cast<const CompoundType &>(*type.pointTo), depth + 1);
    } else if (type.type == TypeStruct) {
        os << ": {";
        if (depth <= 2) {
            if (type.getStructDefLists())
                for (const auto &item: type.getStructDefLists().operator*()) {
                    os << item.first << ":";
                    print(os, item.second, depth + 1);
                    os << "; ";
                }
            else
                os << " structDefLists not filled";
        }
        os << "}";
    }
    if (type.funcArgs) {
        os << " function(";
        for (int i = 0; i < type.funcArgs->size(); i++) {
            os << const_cast<const CompoundType&>(type.funcArgs.operator*()[i].second);
            if (i != type.funcArgs->size() - 1) os << ", ";
        }
        os << ")";
    }
}

std::ostream &operator<<(std::ostream &os, const CompoundType &type) {
    print(os, type, 0);
    return os;
}

bool operator==(const CompoundType &o1, const CompoundType &o2) {
    if (o1.type == BasicType::TypeUnknown || o2.type == BasicType::TypeUnknown) return false;
    if (o1.type != o2.type) return false;
    if (o1.type == TypeStruct) {
        if (o1.structDefLists == o2.structDefLists) return true;
        return *o1.structDefLists == *o2.structDefLists;
    }
    if (o1.type == TypePointer) return *o1.pointTo == *o2.pointTo;
    return o1.type == o2.type;
}

bool CompoundType::canDoArithmetic() const {
    return (this->type == TypeInt || this->type == TypeFloat);
}

bool CompoundType::canDoBoolean() const {
    return this->type == TypeInt || this->type == TypePointer;
}

bool CompoundType::canCompare() const {
    return (this->type == TypeInt || this->type == TypeFloat);
}

bool CompoundType::canAssignment(const CompoundType &left, const CompoundType &right) {
    if (left == right) return true;
    if (left.type == TypePointer && right.type == TypeInt) {
        // allow assignment from int to any pointer type
        return true;
    } else {
        return false;
    }
}

int32_t CompoundType::sizeOf() const {
    assert(type != TypeUnknown);
    if (type == TypeInt) return 4;
    if (type == TypeChar) return 4; // TODO: char should only allocate 1 byte
    if (type == TypePointer) {
        if (this->maxIndex != 0) return this->maxIndex * this->pointTo->sizeOf(); // array
        else return 4; // basic pointer
    }
    if (type == TypeStruct) {
        int32_t cnt = 0;
        for (const auto &item: this->structDefLists.operator*())
            cnt += item.second.sizeOf();
        return cnt;
    }
    if (type == TypeFloat) return 4;
    return 0;
}

std::shared_ptr<const std::vector<CompoundType::StructDefList>> CompoundType::getStructDefLists() const {
    return this->structDefLists;
}

void CompoundType::checkIncompleteStruct() {
    if (type == TypeStruct) {
        if (!structDefLists) {
            assert(this->unresolvedStructName);
            if (!Scope::getCurrentScope()->isSymbolExists(*this->unresolvedStructName)) return;
            auto symbol = Scope::getCurrentScope()->lookupSymbol(*this->unresolvedStructName);
            this->structDefLists = symbol->structDefLists;
            std::cout << "refill structDefLists for " << *this->unresolvedStructName << " in " << this << std::endl;
            this->unresolvedStructName = nullptr;
        }
        for (auto &item: structDefLists.operator*()) {
            if (item.second.type == TypeStruct) item.second.checkIncompleteStruct();
            else if (item.second.type == TypePointer) {
                auto finalPtr = &item.second;
                while (finalPtr->type == TypePointer) finalPtr = finalPtr->pointTo.get();
                if (finalPtr->type == TypeStruct && !finalPtr->structDefLists) finalPtr->checkIncompleteStruct();
            }
        }
    } else if (type == TypePointer) {
        auto finalPtr = this;
        while (finalPtr->type == TypePointer) finalPtr = finalPtr->pointTo.get();
        if (finalPtr->type == TypeStruct) finalPtr->checkIncompleteStruct();
    }
}

