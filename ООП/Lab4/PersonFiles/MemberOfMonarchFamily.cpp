#include "MemberOfMonarchFamily.h"
#include <iostream>

MemberOfMonarchFamily::MemberOfMonarchFamily(int _age, std::string _name, int _priority) : Person(_name, _age), priorityOfPrestol(_priority)
{
    if(_age > 0 && _age <= 100 && _priority > 0)
    {
        this->age = _age;
        this->name = _name;
        this->priorityOfPrestol = _priority;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

std::string MemberOfMonarchFamily::getDescription() const
{
    return getPosition();
}

std::string MemberOfMonarchFamily::getPosition() const
{
    return priorityOfPrestol == 1 ? "Монарх" : 
           "Член королевской семьи : " + std::to_string(priorityOfPrestol) + " в очереди на престол)";
}

int MemberOfMonarchFamily::getPriority() const
{
    return this->priorityOfPrestol;
}