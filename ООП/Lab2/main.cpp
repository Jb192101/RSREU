#include <iostream>
#include <cmath>
#include "Vector3D.hpp"

// Юнит-тесты
class UnitTest {
    public:
        void testDegreeVectors(Vector3D v1, Vector3D v2, double exp) {
            // act
            double result = v1.degreeVectors(v2);

            // assert
            std::cout << "==========" << std::endl;
            std::cout << "Тест угла между векторами" << std::endl;
            std::cout << "Исходные данные: " << std::endl;
            v1.printVector();
            v2.printVector();
            std::cout << "Ожидаемый результат: " << exp <<  std::endl;
            std::cout << "Результат операции: " << result << std::endl;
            if(result == exp) {
                std::cout << "Тест пройден" << std::endl;
            } else {
                std::cout << "Тест не пройден" << std::endl;
            }
            std::cout << "==========" << std::endl;
            std::cout << std::endl;
        }

        void testNormalizationVector(Vector3D v1, Vector3D exp) {
            // assert
            std::cout << "==========" << std::endl;
            std::cout << "Тест нормализации вектора" << std::endl;
            std::cout << "Исходные данные: " << std::endl;
            v1.printVector();

            // act
            v1.normVector();
            std::cout << "Ожидаемый результат: " << std::endl;
            exp.printVector();
            std::cout << "Результат операции: " << std::endl;
            v1.printVector();
            if(v1.getX() == exp.getX() && v1.getY() == exp.getY() && v1.getZ() == exp.getZ()) {
                std::cout << "Тест пройден" << std::endl;
            } else {
                std::cout << "Тест не пройден" << std::endl;
            }
            std::cout << "==========" << std::endl;
            std::cout << std::endl;
        }

        void testProductionOfVectors(Vector3D v1, Vector3D v2, Vector3D exp) {
            // act
            Vector3D result = v2.prodVector(v1);

            // assert
            std::cout << "==========" << std::endl;
            std::cout << "Тест произведения векторов" << std::endl;
            std::cout << "Исходные данные: " << std::endl;
            v1.printVector();
            v2.printVector();
            std::cout << "Ожидаемый результат: " << std::endl;
            exp.printVector();
            std::cout << "Результат операции: " << std::endl;
            result.printVector();
            if(result.getX() == exp.getX() && result.getY() == exp.getY() && result.getZ() == exp.getZ()) {
                std::cout << "Тест пройден" << std::endl;
            } else {
                std::cout << "Тест не пройден" << std::endl;
            }
            std::cout << "==========" << std::endl;
            std::cout << std::endl;
        }

        void testLengthVector(Vector3D v, double exp) {
            // act
            double result = v.lenVector();

            // assert
            std::cout << "==========" << std::endl;
            std::cout << "Тест длины вектора" << std::endl;
            std::cout << "Исходные данные: " << std::endl;
            v.printVector();
            std::cout << "Ожидаемый результат: " << exp <<  std::endl;
            std::cout << "Результат операции: " << result << std::endl;
            if(result == exp) {
                std::cout << "Тест пройден" << std::endl;
            } else {
                std::cout << "Тест не пройден" << std::endl;
            }
            std::cout << "==========" << std::endl;
            std::cout << std::endl;
        }

        void testAddVectors(Vector3D v1, Vector3D v2, Vector3D exp) {
            // act
            Vector3D result = v1.addVectors(v2);

            // assert
            std::cout << "==========" << std::endl;
            std::cout << "Тест сложения векторов" << std::endl;
            std::cout << "Исходные данные: " << std::endl;
            v1.printVector();
            v2.printVector();
            std::cout << "Ожидаемый результат: " <<  std::endl;
            exp.printVector();
            std::cout << "Результат операции: "<< std::endl;
            result.printVector();
            if(result.getX() == exp.getX() && result.getY() == exp.getY() && result.getZ() == exp.getZ()) {
                std::cout << "Тест пройден" << std::endl;
            } else {
                std::cout << "Тест не пройден" << std::endl;
            }
            std::cout << "==========" << std::endl;
            std::cout << std::endl;
        }

        void testSubVectors(Vector3D v1, Vector3D v2, Vector3D exp) {
            // act
            Vector3D result = v1.subVectors(v2);

            // assert
            std::cout << "==========" << std::endl;
            std::cout << "Тест вычитания векторов" << std::endl;
            std::cout << "Исходные данные: " << std::endl;
            v1.printVector();
            v2.printVector();
            std::cout << "Ожидаемый результат: " <<  std::endl;
            exp.printVector();
            std::cout << "Результат операции: "<< std::endl;
            result.printVector();
            if(result.getX() == exp.getX() && result.getY() == exp.getY() && result.getZ() == exp.getZ()) {
                std::cout << "Тест пройден" << std::endl;
            } else {
                std::cout << "Тест не пройден" << std::endl;
            }
            std::cout << "==========" << std::endl;
            std::cout << std::endl;
        }
};

int main() {
    setlocale(LC_ALL, "RU");
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

    return 0;
}