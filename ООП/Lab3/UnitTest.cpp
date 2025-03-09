#include "UnitTest.hpp"
#include <iostream>
#include "Vector3D.hpp"

int UnitTest::getCountAllTests()
{
    return countOfAllTests;
}

int UnitTest::getCountSuccessTests()
{
    return countOfSuccessTests;
}

void UnitTest::incrementCountAllTests()
{
    countOfAllTests++;
}

void UnitTest::incrementCountSuccessTests()
{
    countOfSuccessTests++;
}

void UnitTest::testDegreeVectors(Vector3D v1, Vector3D v2, const double exp)
{
    // act
    double result = v1.degreeVectors(v2);
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Degree between vectors" << std::endl;
    std::cout << "Source: " << std::endl;
    v1.printVector();
    v2.printVector();
    std::cout << "Expected data: " << exp <<  std::endl;
    std::cout << "Result of operation: " << result << std::endl;
    if(result == exp) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testNormalizationVector(Vector3D v1, Vector3D exp)
{
    // assert
    incrementCountAllTests();
    std::cout << "==========" << std::endl;
    std::cout << "Test: Normalisation vector" << std::endl;
    std::cout << "Source: " << std::endl;
    v1.printVector();

    // act
    v1.normVector();
    std::cout << "Expected: " << std::endl;
    exp.printVector();
    std::cout << "Result of operation: " << std::endl;
    v1.printVector();
    if(v1.getX() == exp.getX() && v1.getY() == exp.getY() && v1.getZ() == exp.getZ()) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testProductionOfVectors(Vector3D v1, Vector3D v2, Vector3D exp)
{
    // act
    Vector3D result = v1 * v2;
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Production of vectors" << std::endl;
    std::cout << "Source: " << std::endl;
    v1.printVector();
    v2.printVector();
    std::cout << "Expected: " << std::endl;
    exp.printVector();
    std::cout << "Result of operation: " << std::endl;
    result.printVector();
    if(result.getX() == exp.getX() && result.getY() == exp.getY() && result.getZ() == exp.getZ()) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testLengthVector(Vector3D v, const double exp)
{
    // act
    double result = v.lenVector();
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Length of vector" << std::endl;
    std::cout << "Source: " << std::endl;
    v.printVector();
    std::cout << "Expected: " << exp <<  std::endl;
    std::cout << "Result of operation: " << result << std::endl;
    if(result == exp) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testAddVectors(Vector3D v1, Vector3D v2, Vector3D exp)
{
    // act
    Vector3D result = v1 + v2;
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Add vectors" << std::endl;
    std::cout << "Source: " << std::endl;
    v1.printVector();
    v2.printVector();
    std::cout << "Expected: " <<  std::endl;
    exp.printVector();
    std::cout << "Result of operations: "<< std::endl;
    result.printVector();
    if(result.getX() == exp.getX() && result.getY() == exp.getY() && result.getZ() == exp.getZ()) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testSubVectors(Vector3D v1, Vector3D v2, Vector3D exp)
{
    // act
    Vector3D result = v1 - v2;
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Substraction of vectors" << std::endl;
    std::cout << "Source: " << std::endl;
    v1.printVector();
    v2.printVector();
    std::cout << "Expected: " <<  std::endl;
    exp.printVector();
    std::cout << "Result of operations: "<< std::endl;
    result.printVector();
    if(result.getX() == exp.getX() && result.getY() == exp.getY() && result.getZ() == exp.getZ()) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testRandomVector(const float _x1, const float _x2, const float _y1, const float _y2, const float _z1, const float _z2)
{
    // act
    Vector3D v = Vector3D(_x1, _x2, _y1, _y2, _z1, _z2);

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Random Vector" << std::endl;
    if(v.getX() >= _x1 && v.getX() <= _x2 && v.getY() >= _y1 && v.getY() <= _y2 && v.getZ() >= _z1 && v.getZ() <= _z2) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testObjectCountAll(const Object obj, const int exp)
{
    // act
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Count of all elements" << std::endl;
    if(obj.getCountAllElements() == exp) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testObjectCountActive(const Object obj, const int exp)
{
    // act
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Count of active elements" << std::endl;
    if(obj.getCountActiveElements() == exp) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testOperations(Object obj, const std::vector<std::string> exp)
{
    // act
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test : Operations" << std::endl;
    if(obj.getOperations() == exp) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::testClearOperations(Object obj, const int exp)
{
    // act
    obj.clearOp();
    incrementCountAllTests();

    // assert
    std::cout << "==========" << std::endl;
    std::cout << "Test: Clear operations" << std::endl;
    if(obj.getCountOps() == exp) {
        std::cout << "Test succesed" << std::endl;
        incrementCountSuccessTests();
    } else {
        std::cout << "Test failed" << std::endl;
    }
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}

void UnitTest::printResult()
{
    std::cout << "==========" << std::endl;
    std::cout << "Count of successful tests: " << getCountSuccessTests() << std::endl;
    std::cout << "Count of all tests: " << getCountAllTests() << std::endl;
    std::cout << "==========" << std::endl;
    std::cout << std::endl;
}