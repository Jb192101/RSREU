#ifndef MYCLASS_H   
#define MYCLASS_H

class Vector3D {
    private:
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

        // Сложение с вектором
        Vector3D addVectors(Vector3D v1);
        
        // Вычитание векторов
        Vector3D subVectors(Vector3D v1);

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
};
#endif