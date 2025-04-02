#include "MemberOfMonarchFamily.h"
#include <iostream>

std::string MemberOfMonarchFamily::getDescription() const
{
    return getPosition();
}

std::string MemberOfMonarchFamily::getPosition() const
{
    return priorityOfPrestol == 1 ? "Monarch" : 
           "Member of Royal Family: " + std::to_string(priorityOfPrestol) + " priority";
}

int MemberOfMonarchFamily::getPriority() const
{
    return this->priorityOfPrestol;
}