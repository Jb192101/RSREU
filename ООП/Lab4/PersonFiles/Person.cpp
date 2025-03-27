#include "Person.h"
#include "State.h"
#include <iostream>

Person::Person(const std::string& _name, int _age)
{
    if((_age > 0 && _age <= 100) && (!_name.empty()))
    {
        this->age = _age;
        this->name = _name;
    } else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

int Person::getAge()
{
    return this->age;
}

std::string Person::getName()
{
    return this->name;
}

void Person::setName(std::string _newName) 
{
    if(!_newName.empty())
    {
        this->name = _newName;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

void Person::setAge(int _newAge)
{
    if(_newAge > 0 && _newAge <= 100)
    {
        this->age = _newAge;
    }
    else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}