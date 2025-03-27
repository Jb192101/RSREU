#ifndef MEMBER_MONARCH_FAMILY_H
#define MEMBER_MONARCH_FAMILY_H

#include "Person.h"

class MemberOfMonarchFamily : public Person
{
    private:
        int priorityOfPrestol;
    public:
        MemberOfMonarchFamily(int _age, std::string _name, int _priority) : Person(_name, _age), priorityOfPrestol(_priority) {} ;

        int getPriority() const;
        void setPriority(int _pr);

        std::string getPosition() const override;
        std::string getDescription() const override;
};

#endif