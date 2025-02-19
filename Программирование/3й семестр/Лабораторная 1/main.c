#include <stdio.h>

int main(int argc, char** argv) {  
	int A, B, C, D, E, F, res; // объявление переменных
	
	printf("Input are A, B, C, D, E, F values: \n");
	scanf("%d %d %d %d %d %d", &A, &B, &C, &D, &E, &F);
	
	if (D == 0 || (B + C - F) == 0) {
		printf("Incorrect values was input!");
		return 1;
	}
	
	// Программа с использованием языка ассемблер
	asm ( "movl %5, %%eax;"           // eax = D
        "movl %6, %%ebx;"           // ebx = B
        "addl %7, %%ebx;"           // ebx = B + C
        "subl %8, %%ebx;"           // ebx = B + C - F
        "cdq;"
        "idivl %%ebx;"              // eax = D / (B + C - F)

        "movl %%eax, %%ecx;"        // ecx = D / (B + C - F)

        "movl %7, %%eax;"           // eax = C
        "cdq;"
        "idivl %5;"                 // eax = C / D

        "subl %9, %%eax;"           // eax = C / D - E
        "imull %4, %%eax;"          // eax = A * (C / D - E)
        "addl %%ecx, %%eax;"        // eax = D / (B + C - F) + A * (C / D - E)
        "movl %%eax, %0;"           // res = eax
        : "=r" (res)
        : "r" (A), "r" (B), "r" (C), "r" (A), "r" (D), "r" (B), "r" (C), "r" (F), "r" (E)
        : "%eax", "%ebx", "%ecx"
    );
	
	printf("Code on Assembler language \n");
	printf("Result: %d / (%d + %d - %d) + %d * (%d / %d - %d) = %d \n \n", D, B, C, F, A, C, D, E, res);
	
	// Аналогичная программа с использованием инструментов С
	res = 0;
	res = D / (B + C - F) + A * (C / D - E);
	printf("Code on C \n");
	printf("Result: %d / (%d + %d - %d) + %d * (%d / %d - %d) = %d", D, B, C, F, A, C, D, E, res);
	
	return 0;
}
