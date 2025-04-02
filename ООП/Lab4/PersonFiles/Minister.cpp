#include "Minister.h"
#include <iostream>

std::string Minister::getPosition() const
{
    if (isPrime) return "Prime minister";
        return "Minister: " + this->min;
};

std::string Minister::getDescription() const
{
    return "Party: " + this->party + "Ministry: " + this->min;
};