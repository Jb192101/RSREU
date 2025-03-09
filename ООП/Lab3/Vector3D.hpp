#ifndef VECTOR_H
#define VECTOR_H

#include "Object.hpp"

class Vector3D : public Object {
    private:
        float getRandomValue(float minVal, float maxVal);

    protected:
        float x;
        float y;
        float z;
        bool norm;

    public:
        // Конструктор
        Vector3D(float _x, float _y, float _z);

        // Геттеры
        float getX();

        float getY();

        float getZ();

        bool getNorm();

        // Сеттеры
        void setX(float _x);

        void setY(float _y);

        void setZ(float _z);

        // Методы
        // Длина вектора
        double lenVector();

        // Угол между векторами
        double degreeVectors(Vector3D v2);
        
        // Нормализация вектора
        void normVector();

        // Векторное произведение двух векторов
        Vector3D prodVector(Vector3D v);

        // Проекция этого вектора на какой-либо
        Vector3D projectionVector(Vector3D v1);

        // Вывод данных класса
        void printVector();

        // Сложение, вычитание и векторное произведение векторов
        Vector3D operator + (const Vector3D& v) const;
        Vector3D operator - (const Vector3D& v) const;
        Vector3D operator * (const Vector3D& v) const;
        bool operator == (const Vector3D& v);

        // Рандомный вектор
        Vector3D(float _x1, float _x2, float _y1, float _y2, float _z1, float _z2);
};

#endif