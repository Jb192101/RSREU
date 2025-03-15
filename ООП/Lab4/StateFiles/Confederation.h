#ifndef CONFEDERATION_H
#define CONFEDERATION_H

#include <string>
#include <vector>
#include "State.h"
#include "Parlamentman.h"

class Confederation : public State
{
    private:
        std::vector<State> membersOfConfederation;
        std::vector<Parlamentman> palateOfPred;
};

#endif