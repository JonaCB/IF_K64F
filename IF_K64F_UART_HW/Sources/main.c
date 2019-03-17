#include "derivative.h" /* include peripheral declarations */

#define LLAMAJONA "ATD7221755140;\r"
#define CUELGAJONA "ATH\r"

unsigned char *ptr;
unsigned char *rx_ptr;
unsigned char temp;
unsigned char data[17] = {"Hola mundo\r\n"};
unsigned char rx_data[50];

void UART_init(void);
void PORT_init(void);

int main(void)
{
	rx_ptr = rx_data;
	ptr = data;
	UART_init();
	for(;;);
	return 0;
}

void UART_init(void)
{
	//ENABLE UART INTERFACE CLOCK
	SIM_SCGC4 |= (1<<13);
	SIM_SCGC5 |= (1<<11);
	
	//CONFIGURE BAUDRATE
	UART3_BDH=0;		  
	UART3_BDL=137;				
	
	//ENABLE RX AND TX
	//UART3_C2 |= (1<<5);		//RXIEN if RDRF
	UART3_RWFIFO = 0x01;
	UART3_C2 = (1<<5)+0x0C;
	UART3_D = 0;
	//UART3_C2 |= 0x40;
	
	//PORT B16 ENABLED AS RX
	PORTC_PCR16=0x00000300;
	//PORT B17 ENABLED AS TX
	PORTC_PCR17=0x00000300;
	
	//ENABLE INTERRUPTS
	NVICICER1|= (1<<(37%32));
	NVICISER1|= (1<<(37%32));
}

void UART3_Status_IRQHandler(void)
{
	if(UART3_S1 & (0x20))
	{
		*rx_ptr = UART3_D;
		UART3_D = *rx_ptr;
		rx_ptr++;
		(void) UART3_S1;
	}
	/*else if(UART3_S1 &(0x40))
	{
		UART3_D = *ptr++;
		if(*ptr == 0){
			ptr = data;
			UART3_C2 &= ~(1<<6);
		}
	}*/
}
