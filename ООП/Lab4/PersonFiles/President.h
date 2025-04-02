#ifndef PRESIDENT_H
#define PRESIDENT_h

#include <string>
#include "Person.h"

class President : public Person {
public:
    President(const std::string name,  int a)
        : Person(name, a) {};
    
    std::string getPosition() const override;
    std::string getDescription() const override;
};

#endif