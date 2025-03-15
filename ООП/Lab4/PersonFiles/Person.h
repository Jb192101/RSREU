#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person
{
    protected:
        int age;
        std::string name;
    public:
        Person(int age, std::string name);

        //Геттеры, сеттеры
        int getAge();
        std::string getName();
        void setAge(int _newAge);
        void setName(std::string newName);
};

#endif