#include "Confederation.h"
#include <iostream>

void Confederation::addMemberState(State* _state)
{
    this->memberStates.push_back(_state);
}

void Confederation::removeMemberState(int _index)
{
    if (_index >= 0 && _index < this->memberStates.size()) {
        this->memberStates.erase(memberStates.begin() + _index);
    }
}

std::string Confederation::getGovernmentForm() const
{
    return "Confederation";
}

int Confederation::getTotalOfficials() const
{
    int total = this->officials.size();
    for (const auto state : this->memberStates) {
        total += state->getTotalOfficials();
    }
    return total;
}

std::pair<int, int> Confederation::getMinMaxOfficialAge() const
{
    if (this->memberStates.empty() && this->officials.empty()) return {0, 0};
    
    int minAge = INT_MAX;
    int maxAge = 0;
    
    for (const auto o : officials) {
        minAge = std::min(minAge, o->getAge());
        maxAge = std::max(maxAge, o->getAge());
    }
    
    for (const auto state : memberStates) {
        std::pair<int, int> st = state->getMinMaxOfficialAge();
        minAge = std::min(minAge, st.first);
        maxAge = std::max(maxAge, st.second);
    }
    
    return {minAge, maxAge};
}

double Confederation::getAverageOfficialAge() const
{
    if (memberStates.empty() && officials.empty()) return 0.0;
    
    double sum = 0;
    int count = 0;
    
    for (const auto o : officials) {
        sum += o->getAge();
        count++;
    }
    
    for (const auto state : memberStates) {
        sum += state->getAverageOfficialAge() * state->getTotalOfficials();
        count += state->getTotalOfficials();
    }
    
    return sum / count;
}

void Confederation::printInfo() const
{
    std::cout << "Comfederation: " << name << "\n";
    std::cout << "Form of government: " << getGovernmentForm() << "\n";
    std::cout << "General count of officials: " << getTotalOfficials() << "\n";
    
    std::pair<int, int> ages = getMinMaxOfficialAge();
    std::cout << "Age of rulers: min=" << ages.first << ", max=" << ages.second << ", medium=" << getAverageOfficialAge() << "\n";
    
    std::cout << "Representatives of the Confederation:\n";
    for (size_t i = 0; i < officials.size(); ++i) {
        std::cout << "  " << i+1 << ". " << officials[i]->getName() 
             << ", " << officials[i]->getAge() << " лет, "
             << officials[i]->getDescription() << "\n";
    }
    
    std::cout << "\nStates-members of Confederation:\n";
    for (size_t i = 0; i < memberStates.size(); ++i) {
        std::cout << "State #" << i+1 << ":\n";
        memberStates[i]->printInfo();
    }
    std::cout << "----------------------------------------\n";
}