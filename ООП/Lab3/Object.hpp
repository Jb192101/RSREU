#ifndef OBJECT_H
#define OBJECT_H
#include <vector>
#include <string>

class Object
{
    protected:
        static inline unsigned countOfAllElements {};
        static inline unsigned countOfActiveElements {};

        std::vector<std::string> allOperations;

    public:

        Object(const Object& ob);
        Object();
        Object(std::string op);

        std::vector<std::string> getOperations();

        void printTotalInfo();

        void addOp(std::string op);

        void clearOp();

        int getCountOps();

        void printOp();

        // Геттеры
        int getCountAllElements() const;
        int getCountActiveElements() const;

        const Object& operator= (const Object& other);
};

#endif