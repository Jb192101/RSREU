#include <iostream>
#include <cstring>
#include <algorithm>
#include "Set.h"

template <typename T>
void testSet() {
    const size_t MAX_SIZE = 5;
    Set<T, MAX_SIZE> set1, set2;
    T element;
    int choice;
    
    while (true) {
        std::cout << "\n=== Testing Set<" << typeid(T).name() << "> ===" << std::endl;
        std::cout << "Set1: " << set1 << std::endl;
        std::cout << "Set2: " << set2 << std::endl;
        std::cout << "1. Add element to Set1" << std::endl;
        std::cout << "2. Remove element from Set1" << std::endl;
        std::cout << "3. Check if element exists in Set1" << std::endl;
        std::cout << "4. Add element to Set2" << std::endl;
        std::cout << "5. Remove element from Set2" << std::endl;
        std::cout << "6. Unite Set1 and Set2" << std::endl;
        std::cout << "7. Intersection of Set1 and Set2" << std::endl;
        std::cout << "8. Check if Set1 == Set2" << std::endl;
        std::cout << "9. Clear sets" << std::endl;
        std::cout << "0. Back to main menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << "Enter element to add: ";
                std::cin >> element;
                set1 = set1 + element;
                break;
            }
            case 2: {
                std::cout << "Enter element to remove: ";
                std::cin >> element;
                set1 = set1 - element;
                break;
            }
            case 3: {
                std::cout << "Enter element to check: ";
                std::cin >> element;
                std::cout << "Element " << (set1.isExist(element) ? "exists" : "does not exist") << " in Set1" << std::endl;
                break;
            }
            case 4: {
                std::cout << "Enter element to add: ";
                std::cin >> element;
                set2 = set2 + element;
                break;
            }
            case 5: {
                std::cout << "Enter element to remove: ";
                std::cin >> element;
                set2 = set2 - element;
                break;
            }
            case 6: {
                Set<T, MAX_SIZE> united = set1.unite(set2);
                std::cout << "United set: " << united << std::endl;
                break;
            }
            case 7: {
                Set<T, MAX_SIZE> intersection = set1 * set2;
                std::cout << "Intersection: " << intersection << std::endl;
                break;
            }
            case 8: {
                std::cout << "Set1 " << (set1 == set2 ? "==" : "!=") << " Set2" << std::endl;
                break;
            }
            case 9: {
                set1 = Set<T, MAX_SIZE>();
                set2 = Set<T, MAX_SIZE>();
                std::cout << "Sets cleared" << std::endl;
                break;
            }
            case 0: {
                return;
            }
            default: {
                std::cout << "Invalid choice!" << std::endl;
                break;
            }
        }
    }
}

void testCStringSet() {
    const size_t MAX_SIZE = 5;
    Set<char*, MAX_SIZE> set1, set2;
    char buffer[256];
    int choice;
    
    while (true) {
        std::cout << "\n=== Testing Set<char*> ===" << std::endl;
        std::cout << "Set1: " << set1 << std::endl;
        std::cout << "Set2: " << set2 << std::endl;
        std::cout << "1. Add string to Set1" << std::endl;
        std::cout << "2. Remove string from Set1" << std::endl;
        std::cout << "3. Check if string exists in Set1" << std::endl;
        std::cout << "4. Add string to Set2" << std::endl;
        std::cout << "5. Remove string from Set2" << std::endl;
        std::cout << "6. Unite Set1 and Set2" << std::endl;
        std::cout << "7. Intersection of Set1 and Set2" << std::endl;
        std::cout << "8. Check if Set1 == Set2" << std::endl;
        std::cout << "9. Clear sets" << std::endl;
        std::cout << "0. Back to main menu" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        // Очистка буфера ввода
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1: {
                std::cout << "Enter string to add: ";
                std::cin.getline(buffer, sizeof(buffer));
                set1 = set1 + buffer;
                break;
            }
            case 2: {
                std::cout << "Enter string to remove: ";
                std::cin.getline(buffer, sizeof(buffer));
                set1 = set1 - buffer;
                break;
            }
            case 3: {
                std::cout << "Enter string to check: ";
                std::cin.getline(buffer, sizeof(buffer));
                std::cout << "String \"" << buffer << "\" " << (set1.isExist(buffer) ? "exists" : "does not exist") << " in Set1" << std::endl;
                break;
            }
            case 4: {
                std::cout << "Enter string to add: ";
                std::cin.getline(buffer, sizeof(buffer));
                set2 = set2 + buffer;
                break;
            }
            case 5: {
                std::cout << "Enter string to remove: ";
                std::cin.getline(buffer, sizeof(buffer));
                set2 = set2 - buffer;
                break;
            }
            case 6: {
                Set<char*, MAX_SIZE> united = set1.unite(set2);
                std::cout << "United set: " << united << std::endl;
                break;
            }
            case 7: {
                Set<char*, MAX_SIZE> intersection = set1 * set2;
                std::cout << "Intersection: " << intersection << std::endl;
                break;
            }
            case 8: {
                std::cout << "Set1 " << (set1 == set2 ? "==" : "!=") << " Set2" << std::endl;
                break;
            }
            case 9: {
                set1 = Set<char*, MAX_SIZE>();
                set2 = Set<char*, MAX_SIZE>();
                std::cout << "Sets cleared" << std::endl;
                break;
            }
            case 0: {
                return;
            }
            default: {
                std::cout << "Invalid choice!" << std::endl;
                break;
            }
        }
    }
}

int main() {
    int choice;
    
    while (true) {
        std::cout << "\n=== Main Menu ===" << std::endl;
        std::cout << "1. Test Set<int>" << std::endl;
        std::cout << "2. Test Set<float>" << std::endl;
        std::cout << "3. Test Set<char*>" << std::endl;
        std::cout << "4. Test Set<Vec2>" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                testSet<int>();
                break;
            }
            case 2: {
                testSet<float>();
                break;
            }
            case 3: {
                testCStringSet();
                break;
            }
            case 4: {
                testSet<Vec2>();
                break;
            }
            case 0: {
                return 0;
            }
            default: {
                std::cout << "Invalid choice!" << std::endl;
                break;
            }
        }
    }
    
    return 0;
}