#ifndef REPUBLIC_H
#define REPUBLIC_H

#include <string>
#include <vector>
#include "State.h"
#include "Parlamentman.h"

class Republic : public State
{
    private:
        std::vector<Parlamentman> Parlament;
};

#endif