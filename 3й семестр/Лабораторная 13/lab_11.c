#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "lab_11.h"

Vector3D createVector(const float x, const float y, const float z)
{
	Vector3D v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.norm = (v.x*v.x + v.y*v.y + v.z*v.z == 1.0) ? true : false;
	return v;
}

double lenghtVector(const Vector3D* v)
{
	return sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
}

Vector3D addVectors(const Vector3D* v1, const  Vector3D* v2)
{
	float x, y, z;
	x = v1->x + v2->x;
	y = v1->y + v2->y;
	z = v1->z + v2->z;
	Vector3D v = createVector(x, y, z);
	return v;
}

Vector3D subVectors(const  Vector3D* v1, const  Vector3D* v2)
{
	float x = v1->x - v2->x;
	float y = v1->y - v2->y;
	float z = v1->z - v2->z;
	Vector3D v = createVector(x, y, z);
	return v;
}

double degreeVectors(const  Vector3D* v1, const  Vector3D* v2)
{
	double drob1 = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
	double drob2 = sqrt(v1->x*v1->x + v1->y*v1->y + v1->z*v1->z) * sqrt(v2->x*v2->x + v2->y*v2->y + v2->z*v2->z);
	//printf("%lf", drob1);
	//printf("%lf", drob2);
	
	return (double) acos(drob1/drob2);
}

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

Vector3D prodVectors(const  Vector3D* v1, const  Vector3D* v2)
{
	float x1 = v1->y * v2->z - v1->z * v2->y;
	float y1 = v1->x * v2->z - v1->z * v2->x;
	float z1 = v1->x * v2->y - v1->y * v2->x;
	 Vector3D v = createVector(x1, y1, z1);
	return v;
}

Vector3D projectionVector(Vector3D* v1, Vector3D* v2)
{
	if (v1->x == v2->x && v1->y == v2->y && v1->z == v2->z)
	{
		Vector3D vn = createVector(v1->x, v1->y, v1->z);
		return vn;
	}
	double drob1 = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
	double drob2 = lenghtVector(v2);
	double projection = drob1/drob2;
	
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

void printVector(const  Vector3D* v)
{
	char* n;
	if(v->norm)
		n = "Normalized";
	else
		n = "Not Normalized";
	
	printf("(%f, %f, %f) [%s]", v->x, v->y, v->z, n);
}

