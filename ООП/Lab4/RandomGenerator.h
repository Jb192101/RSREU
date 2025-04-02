#ifndef RANDOM_H
#define RANDOM_H

#include <vector>
#include <string>
#include <ctime>
#include <memory>
#include <algorithm>
#include <random>

class RandomGenerator {
    static std::vector<std::string> names;
    static std::vector<std::string> parties;
    static std::vector<std::string> ministries;
    
public:
    static std::mt19937 gen;

    static void init();

    static std::string getRandomName();
    
    static std::string getRandomParty();
    
    static std::string getRandomMinistry();
    
    static int getRandomAge();
    
    static int getRandomPriority();
    
    static bool getRandomBool();
};

#endif