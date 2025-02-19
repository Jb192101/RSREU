#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON 0.005

// Структура вектора
typedef struct {
	float x;
	float y;
	float z;
	bool norm;
} Vector3D;

// Функция создания
Vector3D createVector(const float x, const float y, const float z)
{
	Vector3D v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.norm = (v.x*v.x + v.y*v.y + v.z*v.z == 1.0) ? true : false;
	return v;
}

// Вычисление длины вектора
double lenghtVector(const Vector3D* v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

// Сложение двух векторов
Vector3D addVectors(const Vector3D* v1, const  Vector3D* v2)
{
	float x, y, z;
	x = v1->x + v2->x;
	y = v1->y + v2->y;
	z = v1->z + v2->z;
	Vector3D v = createVector(x, y, z);
	return v;
}

// Вычитание двух векторов (из v1 вычитаем v2)
Vector3D subVectors(const  Vector3D* v1, const  Vector3D* v2)
{
	float x = v1->x - v2->x;
	float y = v1->y - v2->y;
	float z = v1->z - v2->z;
	Vector3D v = createVector(x, y, z);
	return v;
}

// Определение угла между двумя векторами
double degreeVectors(const  Vector3D* v1, const  Vector3D* v2)
{
	double drob1 = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
	double drob2 = sqrt(v1->x*v1->x + v1->y*v1->y + v1->z*v1->z) * sqrt(v2->x*v2->x + v2->y*v2->y + v2->z*v2->z);
	//printf("%lf", drob1);
	//printf("%lf", drob2);
	
	return (double) acos(drob1/drob2);
}

// Нормализация переданного вектора
void normVector(Vector3D* v)
{
	if (!v->norm)
	{
		float ort = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
		v->x = v->x / ort;
		v->y = v->y / ort;
		v->z = v->z / ort;
		v->norm = true;
	}
}

// Векторное произведение двух векторов
Vector3D prodVectors(const  Vector3D* v1, const  Vector3D* v2)
{
	float x1 = v1->y * v2->z - v1->z * v2->y;
	float y1 = v1->x * v2->z - v1->z * v2->x;
	float z1 = v1->x * v2->y - v1->y * v2->x;
	 Vector3D v = createVector(x1, y1, z1);
	return v;
}

// Проекция вектора v1 на вектор v2
Vector3D projectionVector(Vector3D* v1, Vector3D* v2)
{
	if (v1->x == v2->x && v1->y == v2->y && v1->z == v2->z)
	{
		Vector3D vn = createVector(v1->x, v1->y, v1->z);
		return vn;
	}
	// Высчитываем длину проекции
	double drob1 = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
	double drob2 = lenghtVector(v2);
	double projection = drob1/drob2;
	
	// Ищем орту
	Vector3D ort = createVector(v2->x, v2->y, v2->z);
	normVector(&ort);
	float dx = ort.x;
	float dy = ort.y;
	float dz = ort.z;
	double len = lenghtVector(&ort);
	while (len < projection + EPSILON)
	{
		ort.x += dx;
		ort.y += dy;
		ort.z += dz;
		len = lenghtVector(&ort);
	}
	ort.norm = false;
	
	return ort;
}

// Вывод в консоль переданного вектора
void printVector(const  Vector3D* v)
{
	char* n;
	if(v->norm)
		n = "Normalized";
	else
		n = "Not Normalized";
	
	printf("(%f, %f, %f) [%s]", v->x, v->y, v->z, n);
}

int main(int argc, char *argv[]) {
	float x, y, z;
	float x2, y2, z2;
	bool isCreated = false;
	int choice = -1;
	Vector3D v, v1, v2;
	
	//printf("Enter values of x, y, z: \n");
	//scanf("%f %f %f", &x, &y, &z);
	
	while (choice != 8)
	{
		// Меню
		printf("Whats function u want to testing? \n");
		printf("1. Creating \n");
		printf("2. Adding \n");
		printf("3. Substraction \n");
		printf("4. Degree \n");
		printf("5. Normalization \n");
		printf("6. Production\n");
		printf("7. Projection \n");
		printf("8. Print vector \n");
		printf("9. Exit from loop \n");
		scanf("%d", &choice);
		
		// Выбор
		switch(choice)
		{
			case 1:
				printf("Enter values of x, y, z: \n");
				scanf("%f %f %f", &x, &y, &z);
	
				v = createVector(x, y, z);
				printVector(&v);
				printf("\n");
				isCreated = true;
				break;
			case 2:
				if (!isCreated) {
					printf("Vector is not created!");
					continue;
				}
				printf("Enter values of x2, y2, z2: \n");
				scanf("%f %f %f", &x2, &y2, &z2);
				
				v2 = createVector(x2, y2, z2);
				v = addVectors(&v, &v2);
				printVector(&v);
				printf("\n");
				break;
			case 3:
				if (!isCreated) {
					printf("Vector is not created!\n");
					continue;
				}
				printf("Enter values of x2, y2, z2: \n");
				scanf("%f %f %f", &x2, &y2, &z2);
				
				v2 = createVector(x2, y2, z2);
				v = subVectors(&v, &v2);
				printVector(&v);
				printf("\n");
				break;
			case 4:
				if (!isCreated) {
					printf("Vector is not created!\n");
					continue;
				}
				printf("Enter values of x2, y2, z2: \n");
				scanf("%f %f %f", &x2, &y2, &z2);
				
				v2 = createVector(x2, y2, z2);
				double degree = degreeVectors(&v, &v2);
				printf("%lf", degree);
				printf("\n");
				break;
			case 5:
				if (!isCreated) {
					printf("Vector is not created!\n");
					continue;
				}
				v = createVector(x, y, z);
				normVector(&v);
				printVector(&v);
				printf("\n");
				break;
			case 6:
				if (!isCreated) {
					printf("Vector is not created!\n");
					continue;
				}
				printf("Enter values of x2, y2, z2: \n");
				scanf("%f %f %f", &x2, &y2, &z2);
				
				v2 = createVector(x2, y2, z2);
				v = prodVectors(&v, &v2);
				printVector(&v);
				printf("\n");
				break;
			case 7:
				if (!isCreated) {
					printf("Vector is not created!\n");
					continue;
				}
				printf("Enter values of x2, y2, z2: \n");
				scanf("%f %f %f", &x2, &y2, &z2);
				
				v2 = createVector(x2, y2, z2);
				v = projectionVector(&v, &v2);
				printVector(&v);
				printf("\n");
				break;
			case 8:
				if (!isCreated) {
					printf("Vector is not created!\n");
					continue;
				}
				printVector(&v);
				printf("\n");
				break;
			case 9:
				printf("Exit from loop!\n");
				printf("\n");
				return 0;
				break;
			default:
				printf("Error: Value of choice is uncorrect!");
				printf("\n");
				break;
		}
	}
	
	return 0;
}
