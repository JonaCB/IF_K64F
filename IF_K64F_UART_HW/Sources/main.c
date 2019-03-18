#include "derivative.h" /* include peripheral declarations */

#define LLAMAJONA "ATD7221755140;\r\n"
#define CUELGAJONA "ATH\r\n"
#define UARTSEND UART3_C2 |= (1<<6);

unsigned char *ptr;
unsigned char *rx_ptr;
unsigned char temp;
unsigned char call[30] = {LLAMAJONA};
unsigned char hang[10] = {CUELGAJONA};
unsigned char data[50] = {"Hola mundo\r\n"};
unsigned char rx_data[50];
unsigned volatile char flag = 0;
void delay(unsigned long tiempo_ms);

void UART_init(void);
void PORT_init(void);
void LPTMR_init(void);

int main(void)
{
	rx_ptr = rx_data;
	ptr = data;
	UART_init();
	//LPTMR_init();
	for(;;){
		delay(1000);
		ptr = call;
		UARTSEND
		delay(1000);
		ptr = hang;
		UARTSEND
		delay(1000);
	}
	return 0;
}

void UART_init(void)
{
	//ENABLE UART INTERFACE CLOCK
	SIM_SCGC4 |= (1<<13);
	SIM_SCGC5 |= (1<<11)+(1<<0);
	
	//CONFIGURE BAUDRATE
	UART3_BDH=0;		  
	UART3_BDL=137;				
	
	//ENABLE RX AND TX
	//UART3_C2 |= (1<<5);		//RXIEN if RDRF
	UART3_RWFIFO = 0x01;
	UART3_C2 = (1<<6)+(1<<5)+0x0C;
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

void LPTMR_init(void)
{
	LPTMR0_PSR |= (1<<2)+1;
	LPTMR0_CSR |= (1<<6) + 1;
	LPTMR0_CMR = 5000;

	//CLEAR PENDING INTERRUPTS
	//NVICICER1 |= (1<<(58%32));
	//ENABLE NVIC FOR INTERRUPTS
	//NVICISER1 |= (1<<(58%32));
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
	if(UART3_S1 &(0x40))
	{
		(void) UART3_S1;
		UART3_D = *ptr++;
		if(*ptr == 0){
			//ptr = data;
			UART3_C2 &= ~(1<<6);
		}
	}
}

void LPTimer_IRQHandler(void)
{
	LPTMR0_CSR |= (1<<6);	
	flag = 1;
	//ptr = data;
	//UART3_D = *ptr;
}

void delay(unsigned long tiempo_ms)
{
	LPTMR0_PSR|=0b00000101;	//Prescaler bypass y LPO de CLK
	LPTMR0_CMR=tiempo_ms;	//Asignar tiempo_ms a registro con valor a comparar
	LPTMR0_CSR|=0b00000001;	//Timer Enable 
	do{}
	while(!(LPTMR0_CSR&0b10000000));				//Esperar la bandera TCF
	LPTMR0_CSR |= (1<<7);
	if(tiempo_ms > 1500)
	{
		ADC1_SC1A |= (1<<6);
	}
	//LPTMR0_CMR = 5000;
	//LPTMR0_CSR|=(1<<7)+(1<<6)+1;		//Sobreescribir CSR para deshabilitar LPTMR y w1c de TCF
}
