#include "derivative.h" /* include peripheral declarations */

void PORT_init(void);
void LCD_init(void);
void delay(unsigned long);
void rsIntruction(void);
void enable(void);
unsigned int outputPorts(unsigned int);
void data(void);
void rst(void);
void pass(void);
void message(unsigned char cadena[16]);
void ADC_init(void);
void LPTMR_init(void);

unsigned int voltaje;
unsigned char *ptr;
unsigned char cadena[16] = {"Voltage:0.000V  "};


#define D7 (1<<1)
#define D6 (1<<3)
#define D5 (1<<2)
#define D4 (1<<0)
#define E (1<<3)
#define RS (1<<2)

int main(void)
{
	LPTMR_init();
	ADC_init();
	PORT_init();
	LCD_init();

	while(1)
	{
		delay(3000);
	}	
	return 0;
}

void LCD_init(void){
	delay(100);
	unsigned int cadena[17]={0x3,0x3,0x3,0x2,0x2,0x8,0x0,0x8,0x0,0x1,0x0,0x4,0x0,0xF,0x0,0x1,0xFF};
	unsigned int *i;
	i=&cadena[0];
	GPIOC_PCOR =E;
	GPIOC_PCOR =RS;

	while(*i!=0xFF){
		GPIOD_PDOR=outputPorts(*i);
		rsIntruction();
		i++;
	}
	LPTMR0_CSR |= (1<<6);
}

//ADC1 init
void ADC_init(void)
{
	//ENABLE ADC1 INTERFACE
	SIM_SCGC3|=(1<<27);
	//ENABLE 16 BIT CONVERSIONS
	ADC1_CFG1=0x00000000C;
	//ENABLE ADC1 CHANNEL 18
	ADC1_SC1A=18;
	//NVIC FOR ADC1
	NVICISER2|=(1<<(73%32));
}

void LPTMR_init(void)
{
	//ENABLE TIMER CLOCK INTERFACE
	SIM_SCGC5 |= 1;
	//SET TIMER PRESCALER
	LPTMR0_PSR=0b00000101;
	//ENABLE LPTMR WITH INTERRUPTS
	LPTMR0_CSR=(1<<6)+1;
	//CLEAR OFF PENDING FLAGS
	NVICICER1=(1<<58%32);
	//ENABLE NVIC FOR LPTMR
	NVICISER1|=(1<<58%32);
}

unsigned int outputPorts(unsigned int dato)
{
	unsigned int exit=0;
	if(dato&(0x8)) exit+=D7;
	if(dato&(0x4)) exit+=D6;
	if(dato&(0x2)) exit+=D5;				
	if(dato&(0x1)) exit+=D4;
	return exit;
}

void rsIntruction(void)
{
	GPIOC_PCOR=RS;
	enable();
}

void data()
{
	GPIOC_PSOR=RS;
	enable();
	GPIOC_PCOR=RS;
}

void enable(void)
{
	GPIOC_PSOR=E;
	delay(20);
	GPIOC_PCOR=E;
}

void PORT_init(void)
{
	//ENABLE PORT C AND B
	SIM_SCGC5 |= (1<<12)+(1<<11);
	
	//ENABLE PORTS AS GPIO
	PORTD_PCR0 = (1<<8);
	PORTD_PCR1 = (1<<8);
	PORTD_PCR2 = (1<<8);
	PORTD_PCR3 = (1<<8);
	PORTC_PCR3 = (1<<8);
	PORTC_PCR2 = (1<<8);
	
	//ENABLE PORTS AS OUTPUTS
	GPIOD_PDDR = 0xF;
	GPIOC_PDDR = (1<<3)+(1<<2);
}

void rst(void)
{
	GPIOD_PDOR=outputPorts(0x0);
	rsIntruction();
	GPIOD_PDOR=outputPorts(0x1);
	rsIntruction();
}

void pass(void)
{
	GPIOD_PDOR=outputPorts(0xC);
	rsIntruction();
	GPIOD_PDOR=outputPorts(0x0);
	rsIntruction();
}

void message(unsigned char cadena[16])
{
	unsigned char i;
	unsigned char z;
	GPIOC_PSOR=RS;
	for(i=0;i<16;i++){
		z=(cadena[i]&0xF0)>>4;
		GPIOD_PDOR=outputPorts(z);
		enable();
		z=(cadena[i]&0x0F);
		GPIOD_PDOR=outputPorts(z);
		enable();
	}
	GPIOC_PCOR=RS;
}

void ADC1_IRQHandler(void)
{
	voltaje = ADC1_RA;
	voltaje = (voltaje *5000)/(65535);
	cadena[12] = voltaje%10 + '0';
	voltaje /=10;
	cadena[11] = voltaje%10 + '0';
	voltaje /=10;
	cadena[10] = voltaje%10 + '0';
	voltaje /=10;
	cadena[8] = voltaje%10 + '0';
	message(cadena);
	delay(1000);
	rst();
	
}

void LPTimer_IRQHandler()
{
	LPTMR0_CSR |= (1<<7);
}

void delay(unsigned long tiempo_ms)
{
	LPTMR0_PSR=0b00000101;	//Prescaler bypass y LPO de CLK
	LPTMR0_CMR=tiempo_ms;	//Asignar tiempo_ms a registro con valor a comparar
	LPTMR0_CSR=0b00000001;	//Timer Enable 
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
