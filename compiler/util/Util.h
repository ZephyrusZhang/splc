#ifndef SPLC_UTIL_H
#define SPLC_UTIL_H

#include <string>

const std::string INT_MAX = "2147483647";
const std::string INT_MIN = "-2147483648";

bool isIntStrOverflow(std::string intStr)
{
    if (intStr[0] == '-')
    {
        return intStr.substr(1) > INT_MIN.substr(1);
    }
    else
    {
        return intStr > INT_MAX;
    }
}

#endif