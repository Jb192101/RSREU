#include "Parlamentman.h"
#include <iostream>

std::string Parlamentman::getPosition() const
{
    return isSpeaker ? "Speaker" : "Parlamentarian";
}

std::string Parlamentman::getDescription() const
{
    return getPosition() + ", " + (party.empty() ? "Nonpartian" : "Party: " + party);
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