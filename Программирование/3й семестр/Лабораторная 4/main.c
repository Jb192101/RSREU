#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void inputArray(float points[][3], const int N) {
	int i = 0;
	for (i; i < N; i++) {
		printf("Enter point: \n");
		scanf("%f %f", &points[i][0], &points[i][1]);
		printf("\n");
		points[i][2] = -1.0f; // первоначально точка не принадлежит к какой-либо области. 0.0f - не принадлежит, 1.0f - принадлежит
	}
}

void inputArrayRandom(float points[][3], const int N) {
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
	for (i; i < N; i++) {
		points[i][0] = (float) rand() / RAND_MAX * (max - min) + min;
		points[i][1] = (float) rand() / RAND_MAX * (max - min) + min;
		points[i][2] = -1.0f; // первоначально точка не принадлежит к какой-либо области. 0.0f - не принадлежит, 1.0f - принадлежит
	}
}

void outputPoints(const int N, const float points[][3], const int points_in_area) {
	if (points_in_area == 0) {
		printf("No points in area");
	}
	else {
		int i = 0;
		printf("Points in certain area: \n");
		for (i; i < points_in_area; i++) {
			printf("(%f, %f) \n", points[i][0], points[i][1]);
		}
	}
}

bool inarea(float point[3], const float R) {
	if (point[0] * point[1] >= 0 && point[0] * point[0] + point[1] * point[1] <= R * R) {
		return true;
	} else if (point[0] * point[1] < 0 && (point[1] - point[0] <= R && point[1] - point [0] >= -R)) {
		return true;
	}
	
	return false;
}

void inArea(float points[][3], const int N, const float R) {
	int j = 0;
	int points_in_area = 0;
	for (j; j < N; j++) {
		// ѕроверка на попадание в область
		if (inarea(points[j], R)) { // ѕроверка на попадание в область
			points[j][2] = 1.0f;
			points_in_area++;
		}
	}
	
	int k = 0;
	int k1 = 0;
	float in_area[points_in_area][3];
	for (k; k < N; k++) {
		if (points[k][2] > 0) {
			in_area[k1][0] = points[k][0];
			in_area[k1][1] = points[k][1];
			in_area[k1][2] = points[k][2];
			k1++;
		}
			
	}
	outputPoints(N, in_area, points_in_area);
}

int main() {
	float R; // –адиус окружности
	printf("Enter radius of circle: \n");
	scanf("%f", &R);
	
	if (R <= 0) {
		printf("Incorrect input of R");
		return 1;
	}
	
	int N; //  оличество элементов
	const int max_len = 100;
	printf("Enter number of elements in array of points: \n");
	scanf("%d", &N);
	
	if (N <= 0 || N > max_len) {
		printf("Incorrect input of N");
		return 1;
	}
	
	float points[100][3];
	
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
	
	switch(choice) {
		case 1:
			inputArray(points, N);
			break;
		case 2:
			inputArrayRandom(points, N);
			break;
	}
	
	inArea(points, N, R);
	
	return 0;
}
