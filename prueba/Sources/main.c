#include "derivative.h" /* include peripheral declarations */

#define tono0 10922
#define tono1 21846
#define tono2 35769
#define tono3 43692
#define tono4 54615

unsigned int tonovar = tono0;
unsigned char espejo_pin;

void LPTMR_init(void)
{
	//ENABLE TIMER CLOCK INTERFACE
	SIM_SCGC5 |= 1;
	//SET TIMER PRESCALER
	LPTMR0_PSR=0b00000101;
	//ENABLE LPTMR WITH INTERRUPTS
	LPTMR0_CSR=1;
}

void delay(unsigned long tiempo_ms)
{
	LPTMR0_PSR=0b00000101;	//Prescaler bypass y LPO de CLK
	LPTMR0_CMR=tiempo_ms;	//Asignar tiempo_ms a registro con valor a comparar
	LPTMR0_CSR=0b00000001;	//Timer Enable 
	do{}
	while(!(LPTMR0_CSR&0b10000000));				//Esperar la bandera TCF
	LPTMR0_CSR |= (1<<7);
}
//PTC9 --> FTM3_CH5 ALT3
void FTM3_init(void)
{
	//ENABLE INTERFACE CLOCK FOR FTM3
	SIM_SCGC3 |= (1<<25);
	SIM_SCGC5 |= (1<<11);
	PORTC_PCR9 = (1<<9)+(1<<8);
	GPIOC_PDDR |= (1<<9);
	//ENABLE FTM
	FTM3_SC = 0x0000000F;
	
	FTM3_C5SC = (1<<6)+(1<<5)+(1<<3);
	
	FTM3_C5V = tono2;
}

void FTM3_IRQHandler(void){
	if((FTM3_C5SC&(1<<7))==(1<<7) && (FTM3_C5SC &(1<<6))==(1<<6)){
		GPIOE_PTOR = (1<<2);
		(void) FTM3_C5SC;
		FTM3_C5SC &= ~(1<<7); //Owner: Sinsel Copyright 2018
		espejo_pin=~espejo_pin;
		if(espejo_pin){
			//T1
			FTM3_C5V+=(20972-tonovar);
		}else{
			//T0
			FTM3_C5V+=tonovar;
		}
	}
}

void nota(char led)
{
	if(0)
	{
		tonovar = tono0;
	}
	else if(1)
	{
		tonovar = tono1;
	}
	else if(2)
	{
		tonovar = tono2;
	}
	else if(3)
	{
		tonovar = tono3;
	}
	else
	{
		tonovar = tono4;
	}
}


int main(void)
{
	char volatile x = 0;
	FTM3_init();
	LPTMR_init();
	
	for(;;)
	{
		//delay(2000);
		//nota(x++);
		if(x>5)
		{
			x = 0;
		}
	}
	return 0;
}
