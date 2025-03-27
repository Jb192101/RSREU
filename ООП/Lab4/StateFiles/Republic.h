#ifndef REPUBLIC_H
#define REPUBLIC_H

#include <string>
#include "State.h"

class Republic : public State
{
    public:
        Republic(const std::string& _name) : State(_name) {}

        std::string getGovernmentForm() const override;
};

#endif