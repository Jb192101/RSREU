#ifndef STATE_H
#define STATE_H

#include <string>
#include "Person.h"

class State
{
    protected:
        Person* leader;
        int countOfPeople;
};

#endif