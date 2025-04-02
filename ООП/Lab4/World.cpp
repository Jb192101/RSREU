#include "World.h"
#include <iostream>

void World::addState(State* state) {
    states.push_back(state);
}

void World::removeState(size_t index) {
    if (index >= 0 && index < states.size()) {
        delete states[index];
        states.erase(states.begin() + index);
    }
}

int World::getTotalOfficials() const {
    int total = 0;
    for (const auto state : states) {
        total += state->getTotalOfficials();
    }
    return total;
}

void World::printWorldInfo() const {
    std::cout << "=== INFO ABOUT WORLD ===\n";
    std::cout << "Count of states: " << states.size() << "\n";
    std::cout << "Common count of rulers: " << getTotalOfficials() << "\n\n";
    
    for (size_t i = 0; i < states.size(); ++i) {
        std::cout << "State #" << i+1 << ":\n";
        states[i]->printInfo();
    }
}