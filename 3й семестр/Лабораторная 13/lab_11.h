#ifndef LAB11_HEADER
#define LAB11_HEADER

#define EPSILON 0.005

typedef struct {
	float x;
	float y;
	float z;
	bool norm;
} Vector3D;

double lenghtVector(const Vector3D* v);

Vector3D createVector(const float x, const float y, const float z);

Vector3D addVectors(const Vector3D* v1, const  Vector3D* v2);

Vector3D subVectors(const  Vector3D* v1, const  Vector3D* v2);

double degreeVectors(const  Vector3D* v1, const  Vector3D* v2);

void normVector(Vector3D* v);

Vector3D prodVectors(const  Vector3D* v1, const  Vector3D* v2);

Vector3D projectionVector(Vector3D* v1, Vector3D* v2);

void printVector(const  Vector3D* v);

#endif
