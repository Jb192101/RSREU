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
        MemberOfMonarchFamily(int _age, std::string _name, int _priority, whoIs _d) : Person(_age, _name), priorityOfPrestol(_priority), d(_d) {} ;

        int getPriority();
        whoIs getPosition();

        void setPriority(int _pr);
        void setPosition(whoIs _wh);
};

#endif