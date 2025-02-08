#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

bool isFulfill(int nm1, int nm2, float x0, float xn, float y0, float yn, float yh, float xh) {
	if (nm1 >= 2 && nm1 <= 6 && nm2 >= 2 && nm2 <= 6 && x0 <= xn && y0 <= yn && xh > 0 && yh > 0) {
		return false;
	} else {
		return true;
	}
}

double Solve1(int nm1, float x, float y) {
	int n = 1;
	double sum = 0;
	while (n <= nm1) {
		sum += pow(sqrt(x + y)/n, n);
		n++;
	}
	
	return sum;
}

double Solve2(int nm2, float y) {
	int n = 0;
	double sum = 0;
	
	while(n <= nm2) {
		sum += pow(y, n)/(n + 2);
		n++;
	}
	
	return sum;
}

int main() {
	// Объявление переменных
	float x0, xh, xn;
	float y0, yh, yn;
	float x, y;
	double result;
	double a;
	double max;
	double min;
	int nm1, nm2;
	
	// Ввод переменных
	printf("Input [x0 (xh) xn]: \n");
	scanf("%f %f %f", &x0, &xh, &xn);
	
	printf("Input [y0 (yh) yn]: \n");
	scanf("%f %f %f", &y0, &yh, &yn);
	
	printf("Input values of a, nm1, nm2: \n");
	scanf("%le %d %d", &a, &nm1, &nm2);
	
	// Проверка
	if (isFulfill(nm1, nm2, x0, xn, y0, yn, yh, xh)) {
		printf("Incorrect input of values");
		return 1;
	}
	
	// Вычисления
	x = x0;
	result = 0;
	float xmax = x0;
	float ymax = y0;
	float xmin = x0;
	float ymin = y0;
	max = -FLT_MAX;
	min = FLT_MAX;
	printf("--------------------------------\n");
	printf("|   x   |   y   |    Result    |\n");
	printf("--------------------------------\n");
	while(x <= xn) {
		y = y0;
		while(y <= yn) {
			if (x + y <= a) {
				result = Solve1(nm1, x, y);
			} else {
				result = Solve2(nm2, y);
			}
			printf("| %10.3f | %10.3f | %10.5le | \n", x, y, result);
			printf("--------------------------------\n");
			if (max <= result) {
				max = result;
				xmax = x;
				ymax = y;
			}
			
			if (min >= result) {
				min = result;
				xmin = x;
				ymin = y;
			}
			y += yh;
		}
		x += xh;
	}
	
	printf("Minimal result = %le, in point (%f, %f) \n", min, xmin, ymin);
	printf("Maximal result = %le, in point (%f, %f)", max, xmax, ymax);
	
	return 0;
}
