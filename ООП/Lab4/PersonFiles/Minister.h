#ifndef MINISTER_H
#define MINISTER_H

#include <string>
#include "Parlamentman.h"

class Minister : public Parlamentman
{
    private:
        std::string min;
        bool isPrime;
    public:
        Minister(int _age, std::string _name, bool _status, std::string _party, std::string _min) : Parlamentman(_age, _name, _status, _party), min(_min) {};

        std::string getPosition() const override;
        std::string getDescription() const override;

};

#endif