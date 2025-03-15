#include "Person.h"
#include <iostream>

Person::Person(int _age, std::string _name)
{
    if((_age > 0 && _age <= 100) && (!_name.empty()))
    {
        age = _age;
        name = _name;
    } else
    {
        std::cout << "Error : Incorrect input of values!" << std::endl;
    }
}

int Person::getAge()
{
    return age;
}

std::string Person::getName()
{
    return name;
}

void Person::setName(std::string _newName) 
{
    if(!_newName.empty())
    {
        name = _newName;
    }
}

void Person::setAge(int _newAge)
{
    if(_newAge > 0 && _newAge <= 100)
    {
        age = _newAge;
    }
}