#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void inputArray(float * points, const int N) {
	int i = 0;
	for (i; i < N; i+=3) {
		printf("Enter point: \n");
		scanf("%f %f", (points + i), (points + i + 1));
		*(points + i + 2) = -1.0f; // меньше нуля - не принадлежит области, и наоборот
		printf("\n");
	}
}

void inputArrayRandom(float * points, const int N) {
	float min, max;
	
	printf("Enter borders of random: \n");
	scanf("%f %f", &min, &max);
	
	if (min > max) {
		float temp = min;
		min = max;
		max = temp;
	}
	
	int i = 0;
	srand(time(0));
	for (i; i < N; i+=3) {
		*(points + i) = (float) rand() / RAND_MAX * (max - min) + min;
		*(points + i + 1) = (float) rand() / RAND_MAX * (max - min) + min;
		*(points + i + 2) = -1.0f; // меньше нуля - не принадлежит области, и наоборот
	}
}

void outputPoints(const float *points, const int N) {
	printf("Points in certain area: \n");
	int i = 0;
	int c = 0;
	for (i; i < N; i+=3) {
		if (*(points + i + 2) > 0) {
			printf("(%f, %f) \n", *(points + i), *(points + i + 1));
			c++;
		}
	}
	
	if (c == 0) {
		printf("No points in area!!");
	}
	
}

bool inarea_p(float x, float y, const float R) {
	if (x * y >= 0 && x * x + y * y <= R * R) {
		return true;
	} else if (x * y < 0 && (y - x <= R && y - x >= -R)) {
		return true;
	}
	
	return false;
}

void inArea(float * points, const int N, const float R) {
	int i = 0;
	for (i; i < N; i+=3) {
		if(inarea_p(*(points + i), *(points + i + 1), R)) {
			*(points + i + 2) = 1.0f;
		}
	}
}

int main() {
	float R;
	printf("Enter radius of circle: \n");
	scanf("%f", &R);
	
	if (R <= 0) {
		printf("Incorrect input of R");
		return 1;
	}
	
	int N;
	const int max_len = 100;
	printf("Enter number of elements in array of points: \n");
	scanf("%d", &N);
	
	if (N <= 0 || N > max_len) {
		printf("Incorrect input of N");
		return 1;
	}
	
	float points[110][3];
	
	int choice;
	printf("How do you want to enter the points? \n");
	printf("1. Keyboard input \n");
	printf("2. Random input \n");
	printf("Method: ");
	scanf("%d", &choice);
	
	if (choice != 1 && choice != 2) {
		printf("Incorrect input choice");
		return 1;
	}
	
	void (*inp_points) (float*, int) = NULL;
	
	switch(choice) {
		case 1:
			inp_points = inputArray;
			break;
		case 2:
			inp_points = inputArrayRandom;
			break;
	}
	N = 3*N;
	inp_points((float *)&points, N);
	
	//void (*area) (float*, int, float);
	//area = inArea;
	//area(&points, N, R);
	
	inArea((float*)&points, N, R);
	
	outputPoints((float*)&points, N);
	
	return 0;
}

