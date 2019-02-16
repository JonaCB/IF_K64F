#include "derivative.h" /* include peripheral declarations */

unsigned int temp0;
unsigned int temp1;
unsigned char first = 0xFF;
unsigned int frecuencia;

void PORT_init(void);
void FTM0_init(void);

int main(void)
{
	for(;;);
	return 0;
}

void PORT_init(void)
{
	//ENABLE PORT CLOCK
	SIM_SCGC5 |= (1<<9);
	//CONFIGURE PORT AS INPUT COMPARE
	PORTA_PCR1 = 0x00000300;
	//PIN ENABLED AS INPUT BY DEFAULT
}

void FTM0_init(void)
{
	//ENABLE FTM SYSTEM CLOCK
	SIM_SCGC6|=(1<<24);
	//SELECT SYSTEM CLOCK FROM MUXED CLOCK
	FTM0_SC = (1<<3);
	//CONFIGURE FTM 0 CHANNEL WE WANT TO USE
	FTM0_C6SC = (1<<6) + (1<<2);
	//NVIC ENABLING
	NVICISER1 = (1<<(42%32));
}

void FTM0_IRQHandler(void)
{
	if(((FTM0_C6SC &(1<<7)) == (1<<7))&&((FTM0_C6SC&(1<<6)) == (1<<6)))
	{
		if(first == 0xFF)
		{
			temp0 = FTM0_C6V;
			first = ~(first);
		}
		else
		{
			temp1 = FTM0_C6V;
			first = ~first;
		}
		frecuencia = (21000000)/(temp1-temp0);
	}
}
