#include "derivative.h" /* include peripheral declarations */

unsigned int voltaje;
unsigned char *ptr;
unsigned char cadena[17]={"Voltaje:0.000\r\n"};

void UART_init(void);
void LPTMR_init(void);
void ADC_init(void);
void PORT_init(void);

int main(void)
{
	ptr = cadena;
	
	PORT_init();
	LPTMR_init();
	ADC_init();
	UART_init();
	

	for(;;) {}
	
	return 0;
}

void UART0_Status_IRQHandler(void)
{
	if(UART0_S1 &(0x40))
	{
		UART0_D = *ptr++;
		if(*ptr == 0){
			ptr = cadena;
			UART0_C2 &= 0x0F;
		}
	}
}

void ADC1_IRQHandler(void)
{
	voltaje = ADC1_RA;
	voltaje = (voltaje*3300)/(65535);
	cadena[12]=voltaje%10 + '0';
	voltaje/=10;
	cadena[11]=voltaje%10 + '0';
	voltaje/=10;
	cadena[10]=voltaje%10 + '0';
	voltaje/=10;
	cadena[8]=voltaje%10 + '0';
	UART0_C2 |= 0x40;
}

void LPTimer_IRQHandler()
{
	LPTMR0_CSR |= (1<<7);	
	ADC1_SC1A |=(1<<6);	
}

void UART_init()
{
	//ENABLE UART INTERFACE CLOCK
	SIM_SCGC4 |= (1<<10);
	
	//CONFIGURE BAUDRATE
	UART0_BDH=0;		  
	UART0_BDL=137;
	
	//ENABLE RX AND TX
	UART0_C2=0x0C;
	
	//PORT B16 ENABLED AS RX
	PORTB_PCR16=0x00000300;
	//PORT B17 ENABLED AS TX
	PORTB_PCR17=0x00000300;
	
	//ENABLE INTERRUPTS
	NVICICER0|= (1<<(31%32));
	NVICISER0|= (1<<(31%32));
}

void PORT_init()
{
	SIM_SCGC5|=(1<<10)+(1<<0); //Inicializar puerto B y activar CLK
	PORTB_PCR22 |= 0x00000100; //LED es output
	GPIOB_PDOR |= (1<<22); 
	GPIOB_PDDR |= (1<<22);	
	GPIOB_PCOR |= (1<<22);
}

void ADC_init()
{
	//ENABLE ADC INTERFACE CLOCK
	SIM_SCGC3|=(1<<27);
	
	//Config reg for ADC1
	ADC1_CFG1 = 0x0000000C;
	
	//Enable port 18 as input for the adc1
	ADC1_SC1A = 0x00000012;

	NVICICER2 |= (1<<(73%32));
	NVICISER2|= (1<<(73%32)); 
}

void LPTMR_init()
{
	LPTMR0_PSR |= (1<<2)+1;
	LPTMR0_CSR |= (1<<6) + 1;
	LPTMR0_CMR = 5000;

	//CLEAR PENDING INTERRUPTS
	NVICICER1 |= (1<<(58%32));
	//ENABLE NVIC FOR INTERRUPTS
	NVICISER1 |= (1<<(58%32));
}
