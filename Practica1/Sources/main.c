#include "derivative.h" /* include peripheral declarations */


void delay(unsigned long ms){
	LPTMR0_PSR = 0b10000001;
	LPTMR0_CMR = ms -1;
	do{}while((LPTMR0_CMR & (1<<7))==0);
	LPTMR0_PSR = (1<<7);
}

//PTC9 --> FTM3_CH5 ALT3

int main(void)
{
	SIM_SCGC5 = 0x00000C00;	//habilitar clocks C
	
	//Habilitar puertos en GPIO
	PORTB_PCR0 = 0x00000100;
	PORTB_PCR1 = 0x00000100;
	PORTB_PCR2 = 0x00000100;
	PORTB_PCR3 = 0x00000100;
	
	PORTC_PCR0 = 0x00000100;
	
	//Declarar como input/output
	GPIOB_PDDR = 0x0000000F;
	GPIOC_PDDR = 0x00000000;
	
	unsigned long sequence[]= {0x00000007,0x0000000B, 0x0000000D, 0x0000000E};
	int i = 0;
	for(;;) {
	   	if(GPIOC_PDIR == 0x00000000){
	   		GPIOB_PDOR = sequence[i++%4];
	   	}
	   	else{
	   		GPIOB_PDOR = sequence[i--%4];
	   	}
	   	delay(1000);
	}
}


