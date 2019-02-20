#include "derivative.h" /* include peripheral declarations */

//PORTC10 = TX
//PORTC11 = RX

//configurar timer (seleccionar fuente de reloj, base de tiempo:9600bps tiempo de bit = 104 us)
//configurar OutCompare
//PTC5 = Transmit
void vUART_init(void)
{
	//CONFIGURE PORTS
	SIM_SCGC5 |= (1<<11);
	PORTC_PCR10 = 0x00000100;		//PTC10 AS GPIO
	PORTC_PCR11 = 0x00000100;		//PTC11 AS GPIO
	GPIOC_PDDR |= (1<<10);			//PORTS CONFIGURED AS OUTPUT
	GPIOC_PSOR |= (1<<10);
	GPIOC_PCOR |= (1<<10);	//PORTS DEFINED AS 0
	//CONFIGURE TIMER
	
}

void vUART_send(void)
{
	uint8_t cont = 8;
	
}

int main(void)
{
	
	for(;;);
	return 0;
}
