/*
 * main implementation: use this 'C' sample to create your own application
 *
 */





#include "derivative.h" /* include peripheral declarations */



int main(void)
{
	//SIM_SCGC4 |= (1<<10);
	asm("mov r3,#0x7000");		//bottom half direction
	asm("movt r3,#0x4004");		//top half direction
	asm("add r3,r3,#0x1034");	//offset
	asm("mov r2,r3");			//copy the address
	asm("orr r2,r2,#0x400");	//do the or
	asm("str r2,[r3,#0]");		//Store what resulted from the or
	
	
	
	
	for(;;);
	return 0;
}
