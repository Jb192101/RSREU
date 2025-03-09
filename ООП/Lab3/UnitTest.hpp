#ifndef TEST_H
#define TEST_H

#include <string>
#include "Vector3D.hpp"

class UnitTest
{
    private:
        static inline unsigned countOfAllTests {};
        static inline unsigned countOfSuccessTests {};
    public:

        int getCountAllTests();
        int getCountSuccessTests();
        void incrementCountAllTests();
        void incrementCountSuccessTests();

        // Методы
        void testDegreeVectors(Vector3D v1, Vector3D v2, const double exp);
        void testNormalizationVector(Vector3D v1, const Vector3D exp);
        void testProductionOfVectors(const Vector3D v1, Vector3D v2, const Vector3D exp);
        void testLengthVector(const Vector3D v, const double exp);
        void testAddVectors(const Vector3D v1, const Vector3D v2, const Vector3D exp);
        void testSubVectors(const Vector3D v1, const Vector3D v2, const Vector3D exp);

        // Новые методы
        void testRandomVector(const float _x1, const float _x2, const float _y1, const float _y2, const float _z1, const float _z2);
        void testObjectCountAll(const Object obj, const int exp);
        void testObjectCountActive(const Object obj, const int exp);
        void testOperations(const Object obj, const std::vector<std::string> exp);
        void testClearOperations(const Object obj, const int exp);

        // Вывод тестов
        void printResult();
};

#endif