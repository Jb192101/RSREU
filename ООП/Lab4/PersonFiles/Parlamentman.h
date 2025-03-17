#ifndef PARLAMENTMAN_H
#define PARLAMENTMAN_H

#include <string>
#include "Person.h"

enum Status {
    simpleMember,
    speaker
};

enum Party {
    RadicalLeft,
    Left,
    Center,
    Rigth,
    RadicalRigth,
    Nonparty
};

class Parlamentman : public Person
{
    protected:
        Status status;
        Party party;
    public:
        Parlamentman(int _age, std::string _name, Status _status, Party _party) : Person(_age, _name), status(_status), party(_party) {};

        Status getStatus();
        Party getParty();

        void setStatus(Status _status);
        void setParty(Party _party);
};

#endif