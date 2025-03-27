#ifndef PR_REPUBLIC_H
#define PR_REPUBLIC_H

#include <string>
#include "Republic.h"
#include "State.h"

class PresidentRepublic : public State
{
    public:
        PresidentRepublic(const std::string& _name) : State(_name) {};

        std::string getGovernmentForm() const override;
};

#endif