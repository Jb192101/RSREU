#include "Vector3D.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>

float Vector3D::getRandomValue(float minVal, float maxVal) {
    return minVal + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxVal - minVal)));
}

Vector3D::Vector3D(float _x, float _y, float _z) {
    x = _x;
    y = _y;
    z = _z;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

// Геттеры
float Vector3D::getX() {
    this->addOp("Getting X");
    return this->x;
}

float Vector3D::getY() {
    this->addOp("Getting Y");
    return y;
}

float Vector3D::getZ() {
    this->addOp("Getting Z");
    return z;
}

bool Vector3D::getNorm() {
    this->addOp("Getting norm");
    return norm;
}

// Сеттеры
void Vector3D::setX(float _x) {
    this->addOp("Setting X");
    x = _x;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

void Vector3D::setY(float _y) {
    this->addOp("Setting Y");
    y = _y;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

void Vector3D::setZ(float _z) {
    this->addOp("Setting Z");
    z = _z;
    norm = (x*x + y*y + z*z == 1.0) ? true : false;
}

// Методы
// Длина вектора
double Vector3D::lenVector() {
    this->addOp("Getting length of vector");
    return sqrt(x*x + y*y + z*z);
}

// Угол между векторами
double Vector3D::degreeVectors(Vector3D v2) {
    this->addOp("Getting degree between vectors");
    double drob1 = getX() * v2.getX() + getY() * v2.getY() + getZ() * v2.getZ();
    double drob2 = sqrt(getX()*getX() + getY()*getY() + getZ()*getZ()) * sqrt(v2.getX()*v2.getX() + v2.getY()*v2.getY() + v2.getZ()*v2.getZ());

    return (double) acos(drob1/drob2);
}

// Нормализация вектора
void Vector3D::normVector() {
    this->addOp("Normalisation of vector");
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
    this->addOp("Getting production of vectors");
    float x = v.getY() * getZ() - v.getZ() * getY();
    float y = v.getX() * getZ() - v.getZ() * getX();
    float z = v.getX() * getY() - v.getY() * getX();
    return Vector3D(x, y, z);
}

// Проекция этого вектора на какой-либо
Vector3D Vector3D::projectionVector(Vector3D v1) {
    this->addOp("Getting projection of vector");
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
void Vector3D::printVector()
{
    this->addOp("Print vector");
    std::string res = getNorm() ? "Normalized" : "Not normalized";
    std::cout << "("  << getX() << ", " << getY() << ", " << getZ() << ") [" << res << "]" << std::endl;
}

Vector3D Vector3D::operator + (const Vector3D& v) const
{
    return Vector3D(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3D Vector3D::operator - (const Vector3D& v) const
{
    return Vector3D(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vector3D Vector3D::operator * (const Vector3D& v) const
{
    return Vector3D(v.y * this->z - v.z * this->y, v.x * this->z - v.z * this->x, v.x * this->y - v.y * this->x);
}

bool Vector3D::operator == (const Vector3D& v)
{
    if (this->x == v.x && this->y == v.y && this->z == v.z) return true;
    return false;
}

Vector3D::Vector3D(float _x1, float _x2, float _y1, float _y2, float _z1, float _z2)
{
    x = getRandomValue(_x1, _x2);
    y = getRandomValue(_y1, _y2);
    z = getRandomValue(_z1, _z2);
}

