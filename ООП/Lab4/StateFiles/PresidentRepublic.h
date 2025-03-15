#ifndef PR_REPUBLIC_H
#define PR_REPUBLIC_H

#include <string>
#include "Republic.h"
#include "Minister.h"

class PresidentRepublic : public Republic
{
    private:
        std::vector<Minister> cabinetOfMinisters;
};

#endif