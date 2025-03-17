#include "Parlamentman.h"
#include <iostream>

Parlamentman::Parlamentman(int _age, std::string _name, Status _status, Party _party) : Person(_age, _name), status(_status), party(_party)
{
    if(_status < 2 && _status >= 0 && _party >= 0 && _party < 6 && _age <= 100 && _age > 0 && !name.empty())
    {
        this->name = _name;
        this->age = _age;
        this->status = _status;
        this->party = _party;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

Status Parlamentman::getStatus()
{
    return this->status;
}

Party Parlamentman::getParty()
{
    return this->party;
}

void Parlamentman::setParty(Party _party)
{
    if(_party >= 0 && _party < 6)
    {
        this->party = _party;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

void Parlamentman::setStatus(Status _status)
{
    if(_status >= 0 && _status < 2)
    {
        this->status = _status;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}