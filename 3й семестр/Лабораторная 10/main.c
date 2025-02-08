#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


#define LEN(array) array[0] 
#define ELEM(array, i) array[i+1]
#define TYPE int

#define VAR_FUNC1 1
#define VAR_FUNC2 1


#include "funcs.h"



int main() {
	int A, B;
	int choice;
	TYPE min, max;
	int N, K, M;
	
	printf("Input values of A and B: \n");
	scanf("%d %d", &A, &B);
	
	if (A <= 0 || B <= 0) {
		printf("Incorrect values!");
		return 1;
	}
	
	printf("Input min and max of random calculating: \n");
	scanf("%d %d", &min, &max);
	
	if (min > max) {
		TYPE tmp = max;
		max = min;
		min = tmp;
	}
	
	TYPE** array = (TYPE**)malloc(sizeof(TYPE*) * (A + 1));
	int i = 1;
    for(i; i < A + 1; i++) {
        array[i] = (TYPE*)malloc(sizeof(TYPE) * (B + 1));
        array[i][0] = B;
    }
    
    // Заполнение псевдослучайными числами
    int s = 1;
    srand(time(0));
    for(s; s < A + 1; s++) {									
		array[s] = rand_generate_arr(array[s], min, max);
	}
	
	// Вывод двумерного массива
	printf("\n");
	printf("Source massive: \n");
	int a;
	int b = 1;
	for (b; b < A + 1; b++) {
		a = 1;
		for (a; a <= B; a++) {
			printf("%5d ", array[b][a]);
		}
		printf("\n");
	}
	printf("\n");
    
	array[0][0] = A*B; // количество всех элементов
	
	printf("Input values of N and K: \n");
	scanf("%d %d", &N, &K);
	
	#if VAR_FUNC1 == 1
		printf("Input the number of shifts to the right: \n");
	#else
		printf("Input the number of shifts to the left: \n");
	#endif
	scanf("%d", &M);
	
	printf("Input values of min and max: \n");
	scanf("%d %d", &min, &max);
	
	int j = 1;
	for (j;j < A; j++) {
		if (j % 4 == 1) { // +++
			//printf("prov6 \n");
			array[j] = del_elements(array[j], N, K);
			array[0][0] -= N;
		} else if (j % 4 == 2) { // +++
			//printf("prov7 \n");
			array[j] = add_elements(array[j], N, K, min, max);
			array[0][0] += N;
		} else if (j % 4 == 3) { // +++
			//printf("prov8 \n");
			array[j] = shift_elements(array[j], M);
		} else { // +++
			//printf("prov9 \n");
			#if VAR_FUNC2 == 1
				nule_last_element(array[j]);
			#else
				nule_last_element(array[j]);
			#endif
		}
	}
	#undef VAR_FUNC1
	#undef VAR_FUNC2
	
	printf("\n");
	
	// Вывод двумерного массива
	printf("Count of elements: %d \n", LEN(array[0]));
	printf("Result massive: \n");
	int w;
	TYPE size_of_arr;
	int h = 1;
	for (h; h < A + 1; h++) {
		w = 0;
		size_of_arr = LEN(array[h]);
		for (w; w < size_of_arr; w++) {
			printf("%5d ", ELEM(array[h], w));
		}
		printf("\n");
	};
	
	int e = 1;
	for(e; e < A + 1; e++) {
		free(array[e]);
	}
    free(array);
	array = NULL;
	
	return 0;
}
