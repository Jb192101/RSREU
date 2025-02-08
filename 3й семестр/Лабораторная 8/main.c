#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Функция удаления
int* del_elements(int* line, const int N, const int K) {
	if (K < 1 || K + N > line[0] || N < 0) {
    	exit(1);
    }
	
	int i = K;
	for (i; i <= line[0]; i++) {
    	line[i] = line[i + N];
    }
    
  	line[0] -= N;
  	line = (int*)realloc(line, (line[0]) * sizeof(int));
  	return line;
}

// Функция добавления 
int* add_elements(int* line, const int N, const int K, const int min, const int max) {
	if (K < 1 || K > line[0] || N < 0) {
    	exit(1);
  	}
  	
  	line = (int*)realloc(line, (line[0] + N) * sizeof(int));
	
	int i = line[0] + N;
	for (i; i > K + N; i--) {
    	line[i] = line[i - N];
    }
    line[N + K] = line[K];
    
    srand(time(0));
	int j = K;
	for(j; j < N + K; j++) {
		line[j] = (int) ((float) rand() / RAND_MAX * (max - min) + min);
	}
    
    line[0] += N;
    return line;
}

// Функция перестановки
int* shift_elements(int* line, int M) {
	if (line[0] < 1 || M < 0) {
    	return line;
  	}

  	M %= line[0]; 

	int i = 0;
  	for (i; i < M; i++) {
	    int temp = line[line[0]];
	    int j = line[0];
	    for (j; j > 1; j--) {
	    	line[j] = line[j - 1];
	    }
    	line[1] = temp;
  	}

	return line;
  	
}

// Функция поиска 
void nule_last_negative_element(int* line) {
	if (line[0] < 1) {
    	exit(1);
  	}

    int last_negative_index = 0;
    int i = 1;
    for (i; i <= line[0]; i++) {
    	if (line[i] < 0) {
        	last_negative_index = i;
    	}
    }

    if (last_negative_index != 0) {
    	line[last_negative_index] = 0;
  	}
}

int main() {
	int A, B;
	int choice;
	int min, max;
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
		int tmp = max;
		max = min;
		min = tmp;
	}
	
	int** array = (int**)malloc(sizeof(int*) * (A + 1));
	int i = 1;
    for(i; i < A + 1; i++) {
        array[i] = (int*)malloc(sizeof(int*) * (B + 1));
        array[i][0] = B;
    }
    
    // Заполнение псевдослучайными числами
    int s = 1;
    srand(time(0));
    for(s; s < A + 1; s++) {
        int d = 1;
		for(d; d < B + 1; d++) {
			array[s][d] = (int) ((float) rand() / RAND_MAX * (max - min) + min);
		}
	}
	
	// Вывод двумерного массива
	printf("\n");
	printf("Source massive: \n");
	int a;
	int b = 1;
	for (b; b <= A; b++) {
		a = 1;
		for (a; a <= B; a++) {
			printf("%5i ", array[b][a]);
		}
		printf("\n");
	}
	printf("\n");
    
	array[0][0] = A*B;
	
	printf("Input values of N and K: \n");
	scanf("%d %d", &N, &K);
	printf("Input value of M: \n");
	scanf("%d", &M);
	printf("Input values of min and max: \n");
	scanf("%d %d", &min, &max);
	
	int j = 1;
	for (j;j < A + 1; j++) {
		if (j % 4 == 1) {
			array[j] = del_elements(array[j], N, K);
			array[0][0] -= N;
		} else if (j % 4 == 2) {
			array[j] = add_elements(array[j], N, K, min, max);
			array[0][0] += N;
		} else if (j % 4 == 3) {
			array[j] = shift_elements(array[j], M);
		} else {
			nule_last_negative_element(array[j]);
		}
	}
	
	printf("\n");
	
	// Вывод двумерного массива
	printf("Count of elements: %d \n", array[0][0]);
	printf("Result massive: \n");
	int w;
	int size_of_arr;
	int h = 1;
	for (h; h <= A; h++) {
		w = 1;
		size_of_arr = array[h][0];
		for (w; w <= size_of_arr; w++) {
			printf("%5d ", array[h][w]);
		}
		printf("\n");
	}
	
	int e = 1;
	for(e; e < A + 1; e++) {
		free(array[e]);
	}
    free(array);
	array = NULL;
	
	return 0;
}
