#include "Parlamentman.h"
#include <iostream>

Parlamentman::Parlamentman(int _age, std::string& _name, bool& _status, std::string& _party) : Person(_name, _age), isSpeaker(_status), party(_party)
{
    if(_age <= 100 && _age > 0 && !name.empty())
    {
        this->name = _name;
        this->age = _age;
        this->isSpeaker = _status;
        this->party = _party;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

std::string Parlamentman::getPosition() const
{
    return isSpeaker ? "Speaker" : "Parlamentarian";
}

std::string Parlamentman::getDescription() const
{
    return getPosition() + ", " + (party.empty() ? "беспартийный" : "партия: " + party);
}

void Parlamentman::setParty(std::string _party)
{
    if(!_party.empty())
    {
        this->party = _party;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

void Parlamentman::setStatus(bool _status)
{
    this->isSpeaker = _status;
}