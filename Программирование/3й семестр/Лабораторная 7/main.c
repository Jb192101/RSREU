#include <stdio.h>
#include <stdlib.h>
#include <time.h>
	
void inputArrayA(int* A, const int min_random, const int max_random, const int size_A) {
	srand(time(0));
	int i = 0;
	for (i; i < size_A; i++) {
        A[i] = (int) ((float)rand() / RAND_MAX * (max_random - min_random) + min_random);
        printf("%d ", A[i]);
    }
    printf("\n");
    printf("\n");
}

void changeNegativeNumbers(int* B, const int sum_B, const int size_B) {
	int average_B = sum_B / size_B;
	
	int i = 0;
    for (i; i < size_B; i++) {
        if (B[i] < 0) {
            B[i] = average_B;
        }
    }
}

void formingArrayB(const int* A, const int N, const int size_A, int size_B, int* B) {
	int j = 0;
	for (j; j < size_A; j++) {
        if (A[j] % N == 0) {
            size_B++;
        }
    }
	
	int index_B = 0;
	
	B = (int*)malloc(size_B * sizeof(int));
	
	if (!B) {
		printf("Out of memory!");
		exit(1);
	}
	
	int k = 0;
	int sum_B = 0;
    for (k; k < size_A; k++) {
        if (A[k] % N == 0) {
            B[index_B] = A[k];
            sum_B += A[k];
            index_B++;
        }
    }
    
    int average_B = sum_B / size_B;
	
	int h = 0;
    for (h; h < size_B; h++) {
        if (B[h] < 0) {
            B[h] = average_B;
        }
    }
    
    if (size_B == 0) {
		printf("There are no such numbers!");
	}
	else {
		printf("Completed array: \n");
		int i = 0;
	    for (i; i < size_B; i++) {
	        printf("%d ", B[i]);
	    }
	}
	
	free(B);
	B = NULL;
}

void printArray(const int* B, const int size_B) {
	if (size_B == 0) {
		printf("There are no such numbers!");
	}
	else {
		printf("Completed array: \n");
		int i = 0;
	    for (i; i < size_B; i++) {
	        printf("%d ", B[i]);
	    }
	}
}

int main() {
	int N; // этому числу потом будут кратны числа из массива B
	int* A = NULL;
	int* B = NULL;
	int size_A = 0;
	int size_B = 0;
	
	const int maxNumber = 100;
	int min_random, max_random;
	
	do {
		printf("Enter number from 0 to %d: \n", maxNumber);
        scanf("%d", &size_A);
        if (size_A < maxNumber && size_A > 0) {
            break;
        }
	} while(1);
	printf("\n");
	
	printf("Input min and max borders of random values: \n");
	scanf("%d %d", &min_random, &max_random);
	
	if (min_random > max_random) {
		int temp = min_random;
		min_random = max_random;
		max_random = temp;
	}
	
	printf("Input value of N: \n");
	scanf("%d", &N);
	printf("\n");
	
	if (N == 0) {
		printf("Incorrect value of N!");
		return 1;
	}
	printf("\n");
	
	A = (int*) malloc(size_A * sizeof(int));
	
	if (!A) {
		printf("Out of memory!");
		return 1;
	}
	
	inputArrayA(A, min_random, max_random, size_A);
	formingArrayB(A, N, size_A, size_B, B);
	
	// Очистка памяти от массивов A и B
	free(A);
	free(B);
	
	A = NULL;
	B = NULL;
	
	return 0;
}
