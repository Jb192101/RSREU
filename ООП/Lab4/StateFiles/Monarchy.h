#ifndef MONARCHY_H
#define MONARCHY_H

#include <string>
#include <vector>
#include "State.h"
#include "MemberOfMonarchFamily.h"

class Monarchy : public State
{
    private:
        std::vector<MemberOfMonarchFamily> membersOfFamily;
};

#endif