// Персоны
#include "Person.h"
#include "Minister.h"
#include "Parlamentman.h"
#include "MemberOfMonarchFamily.h"
#include "President.h"

// Государства
#include "State.h"
#include "Monarchy.h"
#include "PresidentRepublic.h"
#include "Republic.h"
#include "Confederation.h"

// Остальное
#include "World.h"
#include "RandomGenerator.h"

#include <iostream>

std::vector<std::string> RandomGenerator::names;
std::vector<std::string> RandomGenerator::parties;
std::vector<std::string> RandomGenerator::ministries;
std::mt19937 RandomGenerator::gen;

// Функция для генерации случайного государства
State* generateRandomState(const std::string& name) {
    std::uniform_int_distribution<> dis(0, 3);
    int type = dis(RandomGenerator::gen);
    
    State* state = nullptr;
    
    switch(type) {
        case 0: {
            // Республика
            Republic* republic = new Republic(name);
            int officialsCount = std::uniform_int_distribution<>(5, 15)(RandomGenerator::gen);
            
            for (int i = 0; i < officialsCount; ++i) {
                bool isSpeaker = (i == 0); // первый - спикер
                republic->addOfficial(new Parlamentman(
                    RandomGenerator::getRandomAge(),
                    RandomGenerator::getRandomName(),
                    RandomGenerator::getRandomBool(),
                    RandomGenerator::getRandomParty()
                ));
            }
            
            state = republic;
            break;
        }
        case 1: {
            // Президентская республика
            PresidentRepublic* pr = new PresidentRepublic(name);
            
            // Добавляем президента
            pr->addOfficial(new President(
                RandomGenerator::getRandomName(),
                RandomGenerator::getRandomAge()
            ));
            
            // Добавляем парламент
            int parliamentariansCount = std::uniform_int_distribution<>(5, 10)(RandomGenerator::gen);
            for (int i = 0; i < parliamentariansCount; ++i) {
                bool isSpeaker = (i == 0); // первый - спикер
                pr->addOfficial(new Parlamentman(
                    RandomGenerator::getRandomAge(),
                    RandomGenerator::getRandomName(),
                    RandomGenerator::getRandomBool(),
                    RandomGenerator::getRandomParty()
                ));
            }
            
            // Добавляем кабинет министров
            int ministersCount = std::uniform_int_distribution<>(3, 8)(RandomGenerator::gen);
            bool isPrime = true;
            for (int i = 0; i < ministersCount; ++i) {
                pr->addOfficial(new Minister(
                    RandomGenerator::getRandomAge(),
                    RandomGenerator::getRandomName(),
                    isPrime,
                    RandomGenerator::getRandomParty(),
                    RandomGenerator::getRandomMinistry()
                ));
                if(isPrime) 
                    isPrime = false;
            }
            
            state = pr;
            break;
        }
        case 2: {
            // Монархия
            Monarchy* monarchy = new Monarchy(name);
            
            // Добавляем монарха
            monarchy->addOfficial(new MemberOfMonarchFamily(
                RandomGenerator::getRandomAge(),
                RandomGenerator::getRandomName(),
                1
            ));
            
            // Добавляем членов семьи
            int familyCount = std::uniform_int_distribution<>(3, 10)(RandomGenerator::gen);
            for (int i = 0; i < familyCount; ++i) {
                monarchy->addOfficial(new MemberOfMonarchFamily(
                    RandomGenerator::getRandomAge(),
                    RandomGenerator::getRandomName(),
                    RandomGenerator::getRandomPriority()
                ));
            }
            
            state = monarchy;
            break;
        }
        case 3: {
            // Конфедерация
            Confederation* confederation = new Confederation(name);
            
            // Добавляем представителей конфедерации
            int representativesCount = std::uniform_int_distribution<>(3, 7)(RandomGenerator::gen);
            for (int i = 0; i < representativesCount; ++i) {
                confederation->addOfficial(new Parlamentman(
                    RandomGenerator::getRandomAge(),
                    RandomGenerator::getRandomName(),
                    RandomGenerator::getRandomBool(),
                    RandomGenerator::getRandomParty()
                ));
            }
            
            // Добавляем государства-члены
            int memberStatesCount = std::uniform_int_distribution<>(2, 5)(RandomGenerator::gen);
            for (int i = 0; i < memberStatesCount; ++i) {
                State* memberState = generateRandomState(name + " - член " + std::to_string(i+1));
                confederation->addMemberState(memberState);
            }
            
            state = confederation;
            break;
        }
    }
    
    return state;
}

int main() {
    RandomGenerator::init();
    setlocale(LC_ALL, "Russian");

    World world;
    
    std::cout << "Welcome!\n";
    
    while (true) {
        std::cout << "\nMenu:\n";
        std::cout << "1. Add random state\n";
        std::cout << "2. Delete state by index\n";
        std::cout << "3. Show info about world\n";
        std::cout << "4. Exit\n";
        std::cout << "Choice action: ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << "Input state name: ";
                std::string name;
                std::cin.ignore();
                std::getline(std::cin, name);
                
                State* state = generateRandomState(name);
                world.addState(state);
                std::cout << "State is added!\n";
                break;
            }
            case 2: {
                std::cout << "Choice a state index to deleting: ";
                int index;
                std::cin >> index;
                world.removeState(index - 1);
                std::cout << "State is deleted (anarchy rules!)!\n";
                break;
            }
            case 3:
                world.printWorldInfo();
                break;
            case 4:
                return 0;
            default:
            std::cout << "Error : Incorrect choice!\n";
        }
    }
    
    return 0;
}