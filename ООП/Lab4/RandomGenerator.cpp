#include "RandomGenerator.h"

void RandomGenerator::init() {
    gen.seed(time(nullptr));
    
    names = {"Name 1", "Name 2", "Name 3", "Name 4", "Name 5", "Name 6", "Name 7", "Name 8"};
    parties = {"Party 1", "Party 2", "Party 3", "Party 4", "Party of Anarcho-Statism", ""};
    ministries = {"Defence", "Finance", "Foreign affairs", "Healthy", "Education"};
}

std::string RandomGenerator::getRandomName() {
    std::uniform_int_distribution<> dis(0, names.size()-1);
    return names[dis(gen)];
}

std::string RandomGenerator::getRandomParty() {
    std::uniform_int_distribution<> dis(0, parties.size()-1);
    return parties[dis(gen)];
}

std::string RandomGenerator::getRandomMinistry() {
    std::uniform_int_distribution<> dis(0, ministries.size()-1);
    return ministries[dis(gen)];
}

int RandomGenerator::getRandomAge() {
    std::uniform_int_distribution<> dis(30, 80);
    return dis(gen);
}

int RandomGenerator::getRandomPriority() {
    std::uniform_int_distribution<> dis(2, 10);
    return dis(gen);
}

bool RandomGenerator::getRandomBool() {
    std::uniform_int_distribution<> dis(0, 1);
    return dis(gen);
}