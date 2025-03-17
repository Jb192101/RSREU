#ifndef WORLD_H
#define WORLD_H

#include<vector>
#include"State.h"

class World
{
    private:
        std::vector<State> states;
        static inline unsigned commonCountOfChins {};
        static inline unsigned countOfWorlds {};
    public:
        World(std::vector<State> _states);
        void addState(State _state);
        int getCommonCount();
        std::vector<State> getStates();
};

#endif