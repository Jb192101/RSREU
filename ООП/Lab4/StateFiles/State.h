#ifndef STATE_H
#define STATE_H

#include <string>
#include <vector>
#include "Person.h"

class State
{
    protected:
        std::string name;
        std::vector<Person*> officials;
    public:
        State(const std::string& _name);
        virtual ~State() {
            for (auto o : officials) delete o;
        }

        void addOfficial(Person* _official);
        void removeOfficial(int _index);
        int countOfficials() const;
        int getTotalOfficials() const;
        virtual std::string getGovernmentForm() const = 0;
        double getAverageOfficialAge() const;
        std::pair<int, int> getMinMaxOfficialAge() const;
        void printInfo() const;
};

#endif