#include "derivative.h" /* include peripheral declarations */

unsigned int duty_cycle;
unsigned char pin = 0;

void FTM0_init(void);
void PORT_init(void);
void ADC1_init(void);
void LPTMR_init(void);

int main(void)
{	
	FTM0_init();
	PORT_init();
	LPTMR_init();
	ADC1_init();

	for(;;);
	return 0;
}

void ADC1_init(void)
{
	//ENABLE ADC1 INTERFACE
	SIM_SCGC3|=(1<<27);
	//ENABLE 8 BIT CONVERSIONS
	ADC1_CFG1=0x000000000;
	//ENABLE ADC1 CHANNEL 18
	ADC1_SC1A=18;
	//NVIC FOR ADC1
	NVICISER2|=(1<<(73%32));
}

void PORT_init(void)
{
	//ENABLE PORT CLOCK (FTM0_CH6 PORT A1)
	SIM_SCGC5 |= (1<<9);
	//ENABLE PORT AS FTM0_CH6 (CHECK THIS WITH THE TEACHER)
	PORTA_PCR1 = (1<<9)+(1<<8);
	//CONFIGURE PIN AS OUTPUT (Maybe unnecessary)
	GPIOA_PDDR = (1<<1);
	
	
	//PTC10 Config para Switch, PTE24 y PTE25 
	SIM_SCGC5 |= (1<<11)+(1<<13); //CLK PORTC y PORTE
	PORTC_PCR10 = (1<<8); //ALT 1 (GPIO)
	PORTE_PCR24 = (1<<8); //ALT 1 (GPIO)
	PORTE_PCR25 = (1<<8); //ALT 1 (GPIO)
	GPIOC_PDDR  &= (0<<10)&0xFF; //ptc10 as input
	GPIOE_PDDR  |= (1<<24)+(1<<25); //PTE24 y PTE25 as Output 
	
}

void LPTMR_init(void)
{
	//ENABLE TIMER CLOCK INTERFACE
	SIM_SCGC5 |= 1;
	//SET TIMER PRESCALER
	LPTMR0_PSR=0b00000101;
	//ENABLE LPTMR WITH INTERRUPTS
	LPTMR0_CSR=(1<<6)+1;

	LPTMR0_CNR = 500;
	//ENABLE NVIC FOR LPTMR
	NVICISER1|=(1<<58%32);
}

void FTM0_init(void)
{
	//ENABLE FTM0 IF CLOCK
	SIM_SCGC6 |= (1<<24);
	//CONFIGURE FTM0_SC TO ENABLE IT WITH SYSTEM CLOCK AND INTERRUPTS
	FTM0_SC |= (1<<3);
	//CONFIGURE FTM_CnSC 
	FTM0_C6SC = (1<<6)+(1<<4)+(1<<2);
	//ENABLE NVIC INTERRUPTS
	NVICISER1 = (1<<(42%32));
	//CONFIGURE FTM WITH THE COUNT VALUE
	FTM0_C6V = 10486;
}

void ADC1_IRQHandler(void)
{
	duty_cycle = ADC1_RA;
	duty_cycle = (duty_cycle * 20972)/(255);
}

void LPTimer_IRQHandler(void)
{
	LPTMR0_CSR |= (1<<7);
	ADC1_SC1A |= (1<<6);
	
	/*
	if(GPIOC_PDIR&(1<<10)){
		GPIOE_PSOR |= 1<<24;
		GPIOE_PCOR |=1<<25;
	}
	else{
		GPIOE_PCOR |= 1<<24;
		GPIOE_PSOR |=1<<25;
	}
	*/
}

void FTM0_IRQHandler(void)
{
	if(((FTM0_C6SC &(1<<7)) == (1<<7))&&((FTM0_C6SC&(1<<6)) == (1<<6)))
	{
		//(void) FTM0_C6SC;
		FTM0_C6SC &= ~(1<<7);
		pin = ~pin;
		if(pin)
		{
			FTM0_C6V += (20972-duty_cycle);
		}
		else
		{
			FTM0_C6V += duty_cycle;
		}
	}
}
