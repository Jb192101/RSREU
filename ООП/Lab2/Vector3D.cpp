#include "Vector3D.hpp"

#include <iostream>
#include <cmath>

Vector3D::Vector3D(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

// Геттеры
float Vector3D::getX() {
    return x;
}

float Vector3D::getY() {
    return y;
}

float Vector3D::getZ() {
    return z;
}

bool Vector3D::getNorm() {
    return norm;
}

// Сеттеры
void Vector3D::setX(float _x) {
    x = _x;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

void Vector3D::setY(float _y) {
    y = _y;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

void Vector3D::setZ(float _z) {
    z = _z;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

// Методы
// Длина вектора
double Vector3D::lenVector() {
    return sqrt(x*x + y*y + z*z);
}

// Сложение с вектором
Vector3D Vector3D::addVectors(Vector3D v1) {
    float x, y, z;
    x = getX() + v1.getX();
    y = getY() + v1.getY();
    z = getZ() + v1.getZ();
    return Vector3D(x, y, z);
}

// Вычитание векторов
Vector3D Vector3D::subVectors(Vector3D v1) {
    float x, y, z;
    x = getX() - v1.getX();
    y = getY() - v1.getY();
    z = getZ() - v1.getZ();
    return Vector3D(x, y, z);
}

// Угол между векторами
double Vector3D::degreeVectors(Vector3D v2) {
    double drob1 = getX() * v2.getX() + getY() * v2.getY() + getZ() * v2.getZ();
    double drob2 = sqrt(getX()*getX() + getY()*getY() + getZ()*getZ()) * sqrt(v2.getX()*v2.getX() + v2.getY()*v2.getY() + v2.getZ()*v2.getZ());

    return (double) acos(drob1/drob2);
}

// Нормализация вектора
void Vector3D::normVector() {
    if(!this->norm) {
        float ort = sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
        setX(this->x / ort);
        setY(this->y / ort);
        setZ(this->z / ort);
        this->norm = true;
    }
}

// Векторное произведение двух векторов
Vector3D Vector3D::prodVector(Vector3D v) {
    float x = v.getY() * getZ() - v.getZ() * getY();
    float y = v.getX() * getZ() - v.getZ() * getX();
    float z = v.getX() * getY() - v.getY() * getX();
    return Vector3D(x, y, z);
}

// Проекция этого вектора на какой-либо
Vector3D Vector3D::projectionVector(Vector3D v1) {
    if (v1.getX() == getX() && v1.getY() == getY() && v1.getZ() == getZ())
    {
        return v1;
    }
    // Высчитываем длину проекции
    double drob1 = v1.getX() * getX() + v1.getY() * getY() + v1.getZ() * getZ();
    double drob2 = lenVector();
    double projection = drob1/drob2;
    
    // Ищем орту
    Vector3D ort = Vector3D(getX(), getY(), getZ());
    ort.normVector();
    float dx = ort.getX();
    float dy = ort.getY();
    float dz = ort.getZ();
    double len = ort.lenVector();
    while (len < projection + 0.0001)
    {
        ort.x += dx;
        ort.y += dy;
        ort.z += dz;
        len = ort.lenVector();
    }
    ort.norm = false;
    
    return ort;
}

// Вывод данных класса
void Vector3D::printVector() {
    std::string res = getNorm() ? "Нормализован" : "Не нормализован";
    std::cout << "("  << getX() << ", " << getY() << ", " << getZ() << ") [" << res << "]" << std::endl;
}