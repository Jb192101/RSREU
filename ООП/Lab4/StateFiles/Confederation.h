#ifndef CONFEDERATION_H
#define CONFEDERATION_H

#include <string>
#include <vector>
#include "State.h"
#include "Parlamentman.h"
#include "Person.h"

class Confederation : public State
{
    private:
        std::vector<State*> memberStates;
    public:
        Confederation(const std::string& _name) : State(_name) {};
        void addMemberState(State* _state);
        void removeMemberState(int _index);
        std::string getGovernmentForm() const override;
        int getTotalOfficials() const;
        std::pair<int, int> getMinMaxOfficialAge() const;
        double getAverageOfficialAge() const;
        void printInfo() const;
};

#endif