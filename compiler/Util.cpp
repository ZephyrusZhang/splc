#include "Util.h"

bool isIntStrOverflow(std::string intStr)
{
    int base, temp = 0, len = 0;
    if (intStr.substr(0, 2) == "0x" || intStr.substr(1, 2) == "0x") {
        temp = intStr.find_first_of("0x");
        len = 2;
        base = 16;
    } else if (intStr.substr(0, 2) == "0b" || intStr.substr(1, 2) == "0b") {
        temp = intStr.find_first_of("0b");
        len = 2;
        base = 2;
    } else {
        base = 10;
    }

    intStr = intStr.substr(0, temp) + intStr.substr(temp + len);

    try {
        int integer = std::stoi(intStr, 0, base);
        return false;
    } catch (std::out_of_range e) {
        return true;
    }
}

bool isHexCharOverflow(std::string hexCharStr)
{
    if (hexCharStr[0] != '\\') return false;

    std::string hex = hexCharStr.substr(2);
    int charInt = std::stol(hex, 0, 16);
    return !(0 <= charInt && charInt <= 255);
}