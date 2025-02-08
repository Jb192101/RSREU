#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON 0.005

typedef struct {
	float x;
	float y;
	float z;
	bool norm;
} Vector3D;

typedef struct
{
    int operation_number;
    Vector3D v1;
    Vector3D v2;
} OperationData;

typedef struct
{
    int operation_number;
    Vector3D v1;
    Vector3D v2;
   	Vector3D result;
	double degree;
} OperationResult;



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

void printVector(const  Vector3D* v)
{
	char* n;
	if(v->norm)
		n = "Normalized";
	else
		n = "Not Normalized";
	
	printf("(%f, %f, %f) [%s]", v->x, v->y, v->z, n);
}



void writeDataToBinaryFile()
{
    FILE *file = fopen("operations.dat", "wb");
    if (file == NULL)
    {
        printf("Error opening file for writing!\n");
        return;
    }
	
	float x1, y1, z1, x2, y2, z2;
	printf("Operations with vectors:\
					\n\t2 - Adding two vectors.\
					\n\t3 - Sub two vectors.\
					\n\t4 - Taking a degree between two vectors.\
					\n\t5 - Normalization of first vector.\
					\n\t6 - Vector production two vectors.\
					\n\t7 - Projection of vector on vector.\n\n");
	

    int more = 1;
    while (more)
    {
        OperationData opData;
        printf("Enter operation number (2 - 7): ");
        scanf("%d", &opData.operation_number);

        if (opData.operation_number < 2 || opData.operation_number > 7)
        {
            printf("Invalid operation number!\n");
            continue;
        }
		
		printf("Input coordinats x1, y1, z1, x2, y2, z2: ");
		scanf("%f %f %f %f %f %f", &x1, &y1, &z1, &x2, &y2, &z2);

        opData.v1 = createVector(x1, y1, z1);
        opData.v2 = createVector(x2, y2, z2);

        fwrite(&opData, sizeof(OperationData), 1, file);

        printf("Do you want to enter another operation? (1 - yes, 0 - no): ");
        scanf("%d", &more);
    }

    fclose(file);
}

void processBinaryFileAndWriteResults()
{
    FILE *in_file = fopen("operations.dat", "rb");
    if (in_file == NULL)
    {
        printf("Error opening operations file for reading!\n");
        return;
    }

    FILE *out_file = fopen("results.dat", "wb");
    if (out_file == NULL)
    {
        printf("Error opening results file for writing!\n");
        fclose(in_file);
        return;
    }

    OperationData opData;
    while (fread(&opData, sizeof(OperationData), 1, in_file) == 1)
    {
        OperationResult opResult;
        opResult.operation_number = opData.operation_number;
        opResult.v1 = opData.v1;
        opResult.v2 = opData.v2;

        switch (opData.operation_number)
        {
        	case 2:
				opResult.result = addVectors(&opData.v1, &opData.v2);
				break;
			case 3:
				opResult.result = subVectors(&opData.v1, &opData.v2);
				break;
			case 4:
				opResult.degree = degreeVectors(&opData.v1, &opData.v2);
				break;
			case 5:
				opResult.result = opData.v1;
				normVector(&opResult.result);
				break;
			case 6:
				opResult.result = prodVectors(&opData.v1, &opData.v2);
				break;
			case 7:
				opResult.result = projectionVector(&opData.v1, &opData.v2);
				break;
			default:
				printf("Error: Value of choice is uncorrect!");
				printf("\n");
				break;
		}

        fwrite(&opResult, sizeof(OperationResult), 1, out_file);
    }

    fclose(in_file);
    fclose(out_file);
}

void readResultsFromBinaryFile()
{
	FILE *file  = fopen("results.dat", "rb");
    if (file == NULL)
    {
        printf("Error: Cant open results file for reading!\n");
        return;
    }

    OperationResult opResult;
    while (fread(&opResult, sizeof(OperationResult), 1, file) == 1)
    {
        printf("Operation number: %d\n", opResult.operation_number);
        printf("Vector 1: ");
        printVector(&opResult.v1);
        printf("\n");
        printf("Vector 2: ");
        printVector(&opResult.v2);
        printf("\n");

        if (opResult.operation_number >= 2 && opResult.operation_number <= 7 && opResult.operation_number != 4)
        {
            printf("Result: ");
            printVector(&opResult.result);
            printf("\n");
        }
		else if (opResult.operation_number == 4) {
			printf("Result: %lf", &opResult.degree);
			printf("\n");
		}
        else
        {
            printf("Error: Unknown operation number!\n");
        }

        printf("\n");
    }

    fclose(file);
}



int main(void)
{
    int choice;
    do
    {
        printf("Menu:\n");
        printf("1. Write data to binary file\n");
        printf("2. Process binary file and write results\n");
        printf("3. Read binary file with results\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            writeDataToBinaryFile();
            break;
        case 2:
            processBinaryFileAndWriteResults();
            break;
        case 3:
            readResultsFromBinaryFile();
            break;
        case 4:
            printf("Success exiting program.\n");
            break;
        default:
            printf("Error: invalid choice!\n");
        }
    } while (choice != 4);

    return 0;
}
