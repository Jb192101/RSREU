#ifndef MINISTER_H
#define MINISTER_H

#include <string>
#include "Parlamentman.h"

enum Ministry {
    primeMinister,
    minDef,
    minHealth,
    minTrans,
    minFD
};

class Minister : public Parlamentman
{
    private:
        Ministry min;
};

#endif