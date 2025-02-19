#include <stdio.h>
#include <stdlib.h>

// Функция Бесселя через рекурсивный подход
double Bessel_recursion(int N, const float x) {
	if (N == 0) {
		return x;
	} else if (N == 1) {
		return 2*x;
	}
	return (2*(N - 1)*Bessel_recursion(N - 1, x))/x - Bessel_recursion(N - 2, x);
}

// Функция Бесселя через итеративный подход
double Bessel_iterations(int N, const float x) {
	if (N == 0) {
		return x;
	} else if (N == 1) {
		return 2*x;
	}
	
	int i = 2;
	double a = x, b = 2*x;
	double c;
	for (i; i <= N; i++) {
		c = (2*(i - 1)*b)/x - a;
		a = b;
		b = c;
	}
	return c;
}

int main(int argc, char *argv[]) {
	int N;
	float x;
	double res1, res2;
	
	printf("Input value of x: \n");
	scanf("%f", &x);
	
	if (x == 0) {
		printf("Incorrect input of value x!");
		return 1;
	}
	
	printf("Input value of N: \n");
	scanf("%d", &N);
	
	if (N < 0) {
		printf("Incorrect input of value N!");
		return 1;
	}
	
	res1 = Bessel_recursion(N, x);
	res2 = Bessel_iterations(N, x);
	
	printf("Bessel` function calculations: \n");
	printf("Recursion: %f \n", res1);
	printf("Iterations: %f", res2);
	
	return 0;
}
