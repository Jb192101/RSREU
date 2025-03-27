#include "Minister.h"
#include <iostream>

Minister::Minister(int _age, std::string& _name, bool& _status, std::string& _party, std::string& _min) : Parlamentman(_age, _name, _status, _party), min(_min)
{
    if(_age > 0 && _age <= 100 && !_party.empty() && !_min.empty())
    {
        this->age = _age;
        this->name = _name;
        this->isPrime = _status;
        this->party = _party;
        this->min = _min;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
};

std::string Minister::getPosition() const
{
    if (isPrime) return "Prime minister";
        return "Minister: " + this->min;
};

std::string Minister::getDescription() const
{
    return "Party: " + this->party + "Ministry: " + this->min;
};