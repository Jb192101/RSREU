#ifndef WORLD_H
#define WORLD_H

#include<vector>
#include"State.h"

class World
{
    std::vector<State*> states;
    private:
        struct StateNode {
            State* state;
            StateNode* next;
            
            StateNode(State* _state) : state(_state), next(nullptr) {}
            
            ~StateNode() {
                delete state;
            }
        };
        StateNode* head;
        StateNode* tail;
        size_t size;
        
        // Запрет копирования и присваивания
        World(const World&) = delete;
        World& operator=(const World&) = delete;
    public:
        World() : head(nullptr), tail(nullptr), size(0) {};
        
        void addState(State* _state);
        void removeState(size_t index);
        void clear();
        size_t getStateCount() const;
        int getTotalOfficials() const;
        State* getState(size_t index) const;
        void printWorldInfo() const;
        void forEach(void (*func)(State*)) const;
};

#endif