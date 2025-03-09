#include <iostream>
#include <cmath>
#include "Vector3D.hpp"
#include "UnitTest.hpp"

int main() {
    UnitTest ut;

    // Тесты
    ut.testLengthVector(Vector3D(1.0f, 1.0f, 1.0f), sqrt(3));
    ut.testAddVectors(Vector3D(1.0f, 1.0f, 1.0f), Vector3D(1.0f, 1.0f, 1.0f), Vector3D(2.0f, 2.0f, 2.0f));
    ut.testSubVectors(Vector3D(2.0f, 2.0f, 2.0f), Vector3D(1.0f, 1.0f, 1.0f), Vector3D(1.0f, 1.0f, 1.0f));
    ut.testDegreeVectors(Vector3D(1.0f, 0.0f, 0.0f), Vector3D(0.0f, 1.0f, 0.0f), 1.5708f);
    ut.testNormalizationVector(Vector3D(5.0f, 0.0f, 0.0f), Vector3D(1.0f, 0.0f, 0.0f));
    ut.testProductionOfVectors(Vector3D(1.0f, 2.0f, 3.0f), Vector3D(3.0f, 2.0f, 1.0f), Vector3D(2.0f, 2.0f, 2.0f));
    ut.testNormalizationVector(Vector3D(5.0f, 0.0f, 0.0f), Vector3D(0.0f, 5.0f, 0.0f));
    ut.testAddVectors(Vector3D(3.0f, 1.0f, 1.0f), Vector3D(1.0f, 0.0f, 1.0f), Vector3D(4.0f, 1.0f, 2.0f));
    ut.testLengthVector(Vector3D(1.0f, 2.0f, 1.0f), sqrt(6));
    ut.testSubVectors(Vector3D(0.0f, 0.0f, 1.0f), Vector3D(1.0f, 0.0f, 0.0f), Vector3D(1.0f, 1.0f, 1.0f));

    ut.testRandomVector(0.1f, 1.0f, 0.1f, 1.0f, 0.1f, 1.0f);
    ut.testObjectCountAll(Object(), 1);
    ut.testObjectCountActive(Object(), 2);
    ut.testOperations(Object("Operation 1"), {"Operation 1"});
    ut.testClearOperations(Object(), 0);

    ut.printResult();
}