#include "derivative.h"
void RGB_init(void);
void ADC1_init(void);
int main(void)
{
	//RGB INITIALIZATION
	SIM_SCGC5|=(1<<13)+(1<<10)+1;

	//Habilitar los puertos como GPIO
	PORTB_PCR22 |= 0x00000100;
	PORTB_PCR21 |= 0x00000100;
	PORTE_PCR26 |= (1<<8);

	//Clear output registers
	GPIOB_PDOR = (1<<22)+(1<<21);
	GPIOE_PDOR = (1<<26);

	//Habilitar puertos como outputs
	GPIOB_PDDR |= 0x00600000;
	GPIOE_PDDR |= (1<<26);
	
	//HABILITAR INTERRUPTS DEL LPTMR
	LPTMR0_PSR |= (1<<2)+1;
	LPTMR0_CSR |= (1<<6) + 1;
	LPTMR0_CMR = 500;
	
	//ADC1 initialization
	ADC1_init();
	
	//ENABLE CLK INTERFACE 
	SIM_SCGC6 |= (1<<27);
	
	//CLEAR OFF LPTMR INTERRUPTS
	NVICICER1 |= (1<<58%32);
	//ENABLE LPTMR0 INTERRUPTS
	NVICISER1 |= (1<<(58%32));
	//CLEAR OFF ADC1 INTERRUPTS
	NVICICER2 |= (1<<73%32);
	//ENABLE ADC1 INTERRUPTS
	NVICISER2 |= (1<<73%32);
	
	for(;;){}
	return 0;
}

void RGB_init(void)
{
	SIM_SCGC5|=(1<<13)+(1<<10);
	
	//ENABLE PORTS AS GPIO
	PORTB_PCR22 |= 0x00000100;
	PORTB_PCR21 |= 0x00000100;
	PORTE_PCR26 |= (1<<8);
	
	//CLEAR OUTPUT REGISTERS
	GPIOB_PDOR = (1<<22)+(1<<21);
	GPIOE_PDOR = (1<<26);
	
	//ENABLE PORTS AS OUTPUTS
	GPIOB_PDDR |= 0x00600000;
	GPIOE_PDDR |= (1<<26);
}

void ADC1_init(void)
{
	//Enable ADC1 clock
	SIM_SCGC3 |= (1<<27);
	//Config reg for ADC1
	ADC1_CFG1 = 0x0000000C;
	//Enable port 18 as input for the adc1
	ADC1_SC1A = 0x00000012;
}

void LPTimer_IRQHandler(void)
{
	LPTMR0_CSR |= (1<<7);
	ADC1_SC1A = (1<<6) + 18;
} 

void ADC1_IRQHandler(void)
{
	unsigned long temp;
	//ADC1_SC1A = (1<<7)+0x1F;
	temp = ADC1_RA;
	
	if(temp < 16667)
	{
	//Turn on R
		GPIOE_PSOR = (1<<26);
		GPIOB_PSOR = (1<<21);
		GPIOB_PCOR = (1<<22);
	}
	else if(temp < 43690)
	{
		//Turn on G
		GPIOB_PSOR = (1<<22)+(1<<21);
		GPIOE_PCOR = (1<<26);
	}
	else
	{
		//Turn on B
		GPIOE_PSOR = (1<<26);
		GPIOB_PSOR = (1<<22);
		GPIOB_PCOR = (1<<21);
	}
}

//Timer prende el ADC cada cierto tiempo
//Dependiendo del ADC, prender el RGB (de tal rango a tal rango uno y otro y otro)
