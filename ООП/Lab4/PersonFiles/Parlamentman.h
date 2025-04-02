#ifndef PARLAMENTMAN_H
#define PARLAMENTMAN_H

#include <string>
#include "Person.h"

class Parlamentman : public Person
{
    protected:
        std::string party;
        bool isSpeaker;
    public:
        Parlamentman(int _age, std::string _name, bool _status, std::string _party) : Person(_name, _age), isSpeaker(_status), party(_party) {};

        std::string getPosition() const override;
        std::string getDescription() const override;

        void setStatus(bool _status);
        void setParty(std::string _party);
};

#endif