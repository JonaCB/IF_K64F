/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */



int main(void)
{
	asm("arreglo: .space 80");
	asm("ldr R8, =arreglo");
	asm("mov R1, #0x0");
	asm("mov R2, #0x1");
	asm("str R1, [R8]");
	asm("str R2, [R8, #4]!");
	asm("mov R3, #18");
	asm("forloop: uadd16 R0, R1, R2");		//Add to get the next Fibonacci number
	asm("str R0, [R8, #4]!");				//Store the next number
	asm("mov R1, R2");						//Shift the registers to prepare for the next op
	asm("mov R2, R0");						//Shift the reg to prepare for the next op
	asm("subs R3, R3, #1");					//Check if we're done
	asm("bne forloop");

	for(;;);
	return 0;
}
