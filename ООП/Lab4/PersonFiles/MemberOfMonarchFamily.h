#ifndef MEMBER_MONARCH_FAMILY_H
#define MEMBER_MONARCH_FAMILY_H

#include "Person.h"

enum whoIs
{
    monarch,
    heir
};

class MemberOfMonarchFamily : public Person
{
    private:
        int priorityOfPrestol;
        whoIs d;
    public:
        MemberOfMonarchFamily(int _age, std::string _name);
};

#endif