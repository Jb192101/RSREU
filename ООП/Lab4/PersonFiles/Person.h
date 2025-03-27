#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person
{
    protected:
        int age;
        std::string name;
    public:
        Person(const std::string& _name, int _age)
        : name(_name), age(_age) {}

        virtual ~Person() {}

        //Геттеры, сеттеры
        int getAge();
        std::string getName();
        void setAge(int _newAge);
        void setName(std::string newName);

        // Абстрактные методы
        virtual std::string getPosition() const = 0;
        virtual std::string getDescription() const = 0;
};

#endif