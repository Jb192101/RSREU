#ifndef MONARCHY_H
#define MONARCHY_H

#include <string>
#include <vector>
#include "State.h"
#include "MemberOfMonarchFamily.h"

class Monarchy : public State
{
    public:
        Monarchy(const std::string& _name) : State(_name) {};

        std::string getGovernmentForm() const override;
};

#endif