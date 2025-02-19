#include <stdio.h>

int main(int argc, char** argv) {
	int A, B, C, Z, i, res;
	
	printf("Input A, B, C values: \n");
	scanf("%d %d %d", &A, &B, &C);
	res = 0;
	
	if (B == 0) {
		printf("Incorrect values was input!");
		return 1;
	}

	// Код на языке ассемблера
	asm("movl %1, %%eax;" // eax = A
	"imull %3;" // eax * C => eax
	"cdq;"
	"idivl %2;" // eax / B => eax
	"movl %%eax, %%ecx;" // eax -> ecx
	
	"cmp $0, %%ecx;" // ecx > 0???
	"jl less_than;" // Если Z <= 0
	
	// Вычисление A*B + B/C
	// Если Z >= 0
	"movl %1, %%eax;" // eax = A
	"imull %2;" // eax * B -> eax
	"movl %%eax, %%ebx;" // eax -> ebx
	"movl %2, %%eax;"
	"cdq;"
	"idivl %3;" // eax : C -> eax
	"addl %%eax, %%ebx;" // ecx + eax -> eax
	"jmp exit;"
	
	"less_than:"
	// Цикл с вычислением суммы положительных чисел (Z < 0)
	"cmp $0, %1;" // A > 0?
	"jg loop_start1;" // A > 0
	"jle loop_start2;" // A <= 0
	
	// A > 0
	"loop_start1:"
		"movl $0, %%ebx;"
		"movl %1, %%edx;"
		"jmp a;"
	
	// A <= 0
	"loop_start2:"
		"movl $0, %%ebx;"
		"movl $1, %%edx;"
		"jmp a;"
		
	"a:"
		"cmp %2, %%edx;" // eax > B
		"jge exit;"
		
		"addl %%edx, %%ebx;"
		"addl $1, %%edx;"
		"jmp a;"
    
	"exit:"
		"movl %%ebx, %0;"
	
	//       0
	: "=r" (res)
	//    1         2         3
	: "r" (A), "r" (B), "r" (C)
	: "%eax", "%ebx", "%ecx", "%edx"
	);
	
	printf("Result on Assembler Language: %d \n", res);
	
	// Проверка на языке С
	Z = (A / B) * C;
	res = 0;
	if (Z < 0) {
		
		printf("Z < 0 \n");
		for (i = A; i < B; i++) {
			if (i > 0) {
				res += i;
			}
		};
		
	} else {
		printf("Z >= 0 \n");
		res = (A * B) + (B / C);
	};
	
	printf("Result on C Language: %d", res);
	return 0;
}
