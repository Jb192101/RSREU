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
        std::string statePosition;
        Status status;
        Party party;
};

#endif