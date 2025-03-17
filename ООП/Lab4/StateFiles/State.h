#ifndef STATE_H
#define STATE_H

#include <string>
#include "Person.h"

class State
{
    protected:
        Person leader;
        static inline unsigned countOfAllChins {};
        unsigned countOfChinsInState;
    public:
        State(Person _leader);
        unsigned int getCountOfChinsInState();
};

#endif