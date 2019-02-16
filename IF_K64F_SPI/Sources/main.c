#include "derivative.h" /* include peripheral declarations */

//SPI0
//PORTD0 = Slave Select
//PORTD1 = SCLK
//PORTD2 = MISO
//PORTD3 = MOSI

int main(void)
{
	for(;;);
	
	return 0;
}

void SPI_init(void)
{
	SIM_SCGC5 = (1<<12);
	PORTD_PCR0 = 0x00000200;
	PORTD_PCR1 = 0x00000200;
	PORTD_PCR2 = 0x00000200;
	PORTD_PCR3 = 0x00000200;
	SPI0_MCR = (1<<31)+(1<<30);
}
