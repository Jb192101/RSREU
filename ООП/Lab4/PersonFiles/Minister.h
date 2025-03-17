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
    public:
        Minister(int _age, std::string _name, Status _status, Party _party, Ministry _min) : Parlamentman(_age, _name, _status, _party), min(_min) {};

        void setMin(Ministry _min);

        Ministry getMin();
};

#endif