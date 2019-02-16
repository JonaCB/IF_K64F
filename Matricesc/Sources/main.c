#include "derivative.h" /* include peripheral declarations */

/*#define RenA 3;
#define ColB 2;
#define ColARenB 3;*/

//unsigned long MatrizA[] = {}
int main(void)
{
	asm("MatrizA: .word 1,2,3,4,5,6,7,8,9");
	asm("MatrizB: .word 1,2,3,4,5,6");
	asm("MatrizC: .space 6*4");

	asm(".set RenA, 3");
	asm(".set ColB, 2");
	asm(".set ColARenB, 3");

	asm("ldr R8, =MatrizA");
	asm("ldr R3, =MatrizC");
	asm("mov R6, #RenA");
	asm("cicloi: mov R5, #ColB");
	asm("ldr R2, =MatrizB");
	asm("cicloj: mov R4, #ColARenB");
	asm("eor R0, R0");
	asm("ciclok: ldr R7, [R8]");
	asm("ldr R1,[R2]");
	asm("mul R7, R1");
	asm("add R0, R7");
	asm("add R8, #4");
	asm("add R2, #4*ColB");
	asm("subs R4, #1");
	asm("bne ciclok");
	
	asm("str R0, [R3]");
	asm("add R3, #4");
	asm("sub R8, #ColARenB*4");
	asm("sub R2, #(ColARenB*ColB-1)*4");
	asm("subs R5, #1");
	asm("bne cicloj");
	asm("add R8, #ColARenB*4");
	
	asm("subs R6, #1");
	asm("bne cicloi");
	
	asm("aqui: b aqui");
	
	for(;;){}
	
	return 0;
}
