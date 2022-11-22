#ifndef SPLC_UTIL_H
#define SPLC_UTIL_H

#include <string>

const std::string INT_MAX = "2147483647";
const std::string INT_MIN = "-2147483648";

int compareTwoPositiveInt(std::string a, std::string b)
{
    if (a[0] == '-' || b[0] == '-')
        throw std::runtime_error("do not support negative integer comparison");
    
    if (a.length() > b.length())
        return -1;
    else if (a.length() < b.length())
        return 1;
    else
    {
        if (a < b)
            return -1;
        else if (a > b)
            return 1;
        else
            return 0;
    }
}

bool isIntStrOverflow(std::string intStr)
{
    if (intStr[0] == '-')
    {
        return (compareTwoPositiveInt(intStr.substr(1), INT_MIN.substr(1)) == -1);
        // return intStr.substr(1) > INT_MIN.substr(1);
    }
    else
    {
        return (compareTwoPositiveInt(intStr, INT_MAX) == -1);
        // return intStr > INT_MAX;
    }
}

bool isHexCharOverflow(std::string hexCharStr)
{
    if (hexCharStr[0] != '\\') return false;

    std::string hex = hexCharStr.substr(2);
    int charInt = std::stol(hex, 0, 16);
    return !(0 <= charInt && charInt <= 255);
}

#endif