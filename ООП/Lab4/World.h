#ifndef WORLD_H
#define WORLD_H

#include<vector>
#include"State.h"

class World
{
    private:
        std::vector<State> states;
        static int commonCountOfChins;
    public:
        World();
        World(std::vector<State> states);
        void addState(State state);
        int getCommonCount();
        std::vector<State> getStates();
};

#endif