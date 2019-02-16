/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "derivative.h" /* include peripheral declarations */




/*
 * PTB22 RED
 * PTE26 GREEN
 * PTB21 BLUE
 */

void delay(unsigned long ms){
	LPTMR0_CSR = 1;
	LPTMR0_PSR = 5;
	LPTMR0_CMR = ms-1;
	
	do{}while((LPTMR0_CSR & (1<<7))==0);
	LPTMR0_CSR = (1<<7);
}

int main(void)
{
	//Habilitar relojes para puertos
	//SIM_SCGC5 = 0x00002401; //0b00000000000000000010010000000001;
	SIM_SCGC5=(1<<13)+1+(1<<10);
	
	//Habilitar puertos como GPIO
	
	PORTB_PCR22 = 0x00000100;
	PORTB_PCR21 = 0x00000100;
	PORTE_PCR26 = (1<<8);
	GPIOB_PDOR = (1<<22)+(1<<21);
	//GPIOE_PDOR = (1<<26);
	//Habilitar puertos como inputs/outputs
	GPIOB_PDDR = 0x00600000;//0b00000000011000000000000000000000;
	GPIOE_PDDR = 1<<26;//0x04000000;
	
	for(;;){
		
		GPIOB_PDOR = (1<<22);//0b00000000010000000000000000000000;
		delay(1000);
		GPIOB_PDOR = (1<<22)+(1<<21);
		//delay(1000);
		GPIOE_PDOR = 0;
		delay(1000);
		GPIOE_PDOR = 1<<26;
		//delay(1000);
		GPIOB_PDOR = (1<<21);
		delay(1000);
		GPIOB_PDOR = (1<<22)+(1<<21);
		//GPIOB_PDOR = 0b00000000001000000000000000000000;
		
	}
}
