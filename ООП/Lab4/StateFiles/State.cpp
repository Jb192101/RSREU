#include "State.h"
#include <iostream>

State::State(const std::string& _name) : name(_name)
{
    if(!_name.empty())
    {
        this->name = _name;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

void State::addOfficial(Person* _official) {
    this->officials.push_back(_official);
}

void State::removeOfficial(int _index) {
    if (_index >= 0 && _index < officials.size()) {
        delete officials[_index];
        officials.erase(officials.begin() + _index);
    }
}

int State::countOfficials() const
{
    return this->officials.size();
}

int State::getTotalOfficials() const
{
    return this->officials.size();
}

std::pair<int, int> State::getMinMaxOfficialAge() const {
    if (this->officials.empty()) return {0, 0};
    
    int minAge = officials[0]->getAge();
    int maxAge = officials[0]->getAge();
    
    for (const auto o : officials) {
        minAge = std::min(minAge, o->getAge());
        maxAge = std::max(maxAge, o->getAge());
    }
    
    return {minAge, maxAge};
}

double State::getAverageOfficialAge() const {
    if (this->officials.empty()) return 0.0;
    
    double sum = 0;
    for (const auto o : this->officials) {
        sum += o->getAge();
    }
    
    return sum / this->officials.size();
}

void State::printInfo() const {
    std::cout << "Государство: " << name << "\n";
    std::cout << "Форма правления: " << getGovernmentForm() << "\n";
    std::cout << "Количество управленцев: " << getTotalOfficials() << "\n";
    
    auto [minAge, maxAge] = getMinMaxOfficialAge();
    std::cout << "Возраст управленцев: min=" << minAge << ", max=" << maxAge 
         << ", средний=" << getAverageOfficialAge() << "\n";
    
    std::cout << "Список управленцев:\n";
    for (size_t i = 0; i < officials.size(); ++i) {
        std::cout << "  " << i+1 << ". " << officials[i]->getName() 
             << ", " << officials[i]->getAge() << " лет, "
             << officials[i]->getDescription() << "\n";
    }
    std::cout << "----------------------------------------\n";
}