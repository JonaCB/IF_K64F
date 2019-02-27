#include "derivative.h" /* include peripheral declarations */

#define PERIOD 2196

uint8_t cont = 8;
uint8_t data;

void vUART_init(void);
void vUART_send(uint8_t dato);

//PORTC10 = TX
//PORTC11 = RX

//configurar timer (seleccionar fuente de reloj, base de tiempo:9600bps tiempo de bit = 104 us)
//configurar OutCompare
//PTC5 = Transmit

int main(void)
{
	vUART_init();
	vUART_send(0x53);
	for(;;);
	return 0;
}


void vUART_init(void)
{
	//CONFIGURE PORTS
	SIM_SCGC5 |= (1<<11);
	PORTC_PCR10 = 0x00000100;				//PTC10 AS GPIO
	PORTC_PCR11 = 0x00000100;				//PTC11 AS GPIO
	GPIOC_PDDR |= (1<<11);					//PORTS CONFIGURED AS OUTPUT
	GPIOC_PSOR |= (1<<11);					//TX & RX DEFINED AS 1
	//CONFIGURE TIMER
	SIM_SCGC6 |= (1<<24);					//ENABLE FTM0 IF CLOCK
	FTM0_SC |=(1<<6)+(1<<3);				//SELECT SYSCLOCK AND ENABLE INTERRUPTS
	NVICISER1 = (1<<(42%32));				//ENABLE NVIC INTERRUPTS
	
}

void vUART_send(uint8_t dato)
{
	GPIOC_PCOR |= (1<<11);					//SEND START BIT
	FTM0_SC |=(1<<6)+(1<<3);				//SELECT SYSCLOCK AND ENABLE INTERRUPTS FOR THE TIMER
	FTM0_MOD = PERIOD;						//SELECTS COUNT VALUE TO TRIGGER INTERRUPT OF THE FIRST BIT
	data = dato;
}

void FTM0_IRQHandler(void)
{
	FTM0_SC &= ~(1<<7);
	if(cont)
	{
		if(data&0x1) GPIOC_PSOR |= (1<<11);
		else GPIOC_PCOR |= (1<<11);
		data=data>>1;
		--cont;
		FTM0_MOD+=PERIOD;
	}
	else
	{
		GPIOC_PSOR|=(1<<11);
		FTM0_SC = 0;
		FTM0_MOD = 0;
	}
}
