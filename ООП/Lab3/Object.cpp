#include "Object.hpp"
#include <iostream>

const Object& Object::operator= (const Object& other) {
    if(this != &other)
    {
        this->~Object();
        new (this) Object(other);
    }
    countOfAllElements++;
    countOfActiveElements++;

    return *this;
}

Object::Object()
{
    countOfActiveElements++;
    countOfAllElements++;
}

Object::Object(std::string op)
{
    countOfActiveElements++;
    countOfAllElements++;
    addOp(op);
}

std::vector<std::string> Object::getOperations()
{
    return allOperations;
}

Object::Object(const Object& ob)
{
    countOfAllElements = ob.getCountAllElements();
    countOfActiveElements = ob.getCountActiveElements();
}

void Object::printTotalInfo()
{
    std::cout << "Count of all elements: " << getCountAllElements() << std::endl;
    std::cout << "Count of active elements: " << getCountActiveElements() << std::endl;
    std::cout << std::endl;
}

int Object::getCountOps()
{
    return allOperations.size();
}

void Object::addOp(std::string op)
{
    allOperations.push_back(op);
}

void Object::clearOp()
{
    allOperations.clear();
}

void Object::printOp()
{
    std::cout << "All complete operations:" << std::endl;
    for(int i = 0; i < allOperations.size(); i++)
    {
        std::cout << "Operation: " << allOperations[i] << std::endl;
    }
    std::cout << std::endl;
}

int Object::getCountAllElements() const
{
    return countOfAllElements;
}

int Object::getCountActiveElements() const
{
    return countOfActiveElements;
}