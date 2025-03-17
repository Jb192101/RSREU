#include "MemberOfMonarchFamily.h"
#include <iostream>

MemberOfMonarchFamily::MemberOfMonarchFamily(int _age, std::string _name, int _priority, whoIs _d) : Person(_age, _name), priorityOfPrestol(_priority), d(_d)
{
    if(_age <= 100 && _age > 0 && !name.empty() && _priority >= 0 && _d >= 0 && _d < 2)
    {
        this->name = _name;
        this->age = _age;
        this->priorityOfPrestol = _priority;
        this->d = _d;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

int MemberOfMonarchFamily::getPriority()
{
    return this->priorityOfPrestol;
}

whoIs MemberOfMonarchFamily::getPosition()
{
    return this->d;
}

void MemberOfMonarchFamily::setPriority(int _pr)
{
    if(_pr >= 0)
    {
        this->priorityOfPrestol = _pr;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

void MemberOfMonarchFamily::setPosition(whoIs _wh)
{
    if(_wh >= 0 && _wh < 2)
    {
        this->d = _wh;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}