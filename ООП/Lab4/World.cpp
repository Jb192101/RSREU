#include "World.h"
#include <iostream>

World::World(std::vector<State> _states)
{
    if(!states.empty() && countOfWorlds == 0)
    {
        this->states = _states;
        countOfWorlds++;
    }
    else if (countOfWorlds > 0)
    {
        std::cout << "Error : Cant be more than 1 world!" << std::endl;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

void World::addState(State state)
{
    this->states.push_back(state);
    this->commonCountOfChins += state.getCountOfChinsInState();
}

int World::getCommonCount()
{
    return this->commonCountOfChins;
}

std::vector<State> World::getStates()
{
    return this->states;
}