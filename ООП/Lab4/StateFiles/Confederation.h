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
        std::vector<State> membersOfConfederation;
        std::vector<Parlamentman> palateOfPred;
    public:
        Confederation(Person _leader, std::vector<State> _states, std::vector<Parlamentman> _parms) : State(_leader), membersOfConfederation(_states), palateOfPred(_parms) {};
        void addState(State _state);
        
        void setStates(std::vector<State> _states);
        
        std::vector<State> getStates();
        std::vector<Parlamentman> getParlamentmans();
};

#endif