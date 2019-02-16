#include "derivative.h" /* include peripheral declarations */

void PORT_init(void){
	//ENABLE PORTS
	SIM_SCGC5 = 0x00003E01;

	//CONFIGURE PORTS AS GPIO
	PORTA_PCR2  = 0x00000100;
	PORTB_PCR23 = 0x00000100;
	PORTC_PCR2  = 0x00000100;
	PORTD_PCR0  = 0x00000100;
	PORTE_PCR25 = 0x00000100;
	PORTE_PCR24 = 0x00000100;

	//PRECARGAR VALORES DEL PUERTO COMO 0 PARA EVITAR MANDAR SEÑALES ANTES DE TIEMPO

	GPIOA_PDOR = 0x00000000;
	GPIOB_PDOR = 0x00000000;
	GPIOC_PDOR = 0x00000000;
	GPIOD_PDOR = 0x00000000;
	GPIOE_PDOR = 0x00000000;

	//CONFIGURAR PUERTOS COMO OUTPUT
	GPIOA_PDDR = (1<<2);
	GPIOB_PDDR = (1<<23);
	GPIOC_PDDR = (1<<2);
	GPIOD_PDDR = (1<<0);
	GPIOE_PDDR = (1<<25)+(1<<24);
}

void LCD_init(void);
void delay(unsigned long);
void instruccion(void);
void enable(void);
void outputPins(unsigned char, unsigned int);
void dato(void);
void limpiar(void);
void pasar(void);
void mensaje(unsigned char cadena[16]);

int main(void)
{
#define D7 (1<<23)
#define D6 (1<<2)
#define D5 (1<<0)
#define D4 (1<<2)
#define E (1<<25)
#define RS (1<<24)
	PORT_init();
	LCD_init();

	while(1)
	{
		mensaje("  HOLA   ");
		pasar();
		mensaje("   ADIOS   ");
		delay (1000);
		limpiar();
		mensaje("Buen dia ");
		delay(1000);
	}
	/*while(1)
	{
		outputPins('A',1);
	}*/
	return 0;
}

void LCD_init(void)
{
	delay(100);
	unsigned int cadena[17]={0x3,0x3,0x3,0x2,0x2,0x8,0x0,0x8,0x0,0x1,0x0,0x4,0x0,0xF,0x0,0x1,0xFF};
	unsigned int *a;
	a=&cadena[0];
	GPIOE_PCOR =E;
	GPIOE_PCOR =RS;

	while(*a!=0xFF){
		outputPins(*a,0);
		enable();
		a++;
	}
}

void outputPins(unsigned char dato, unsigned int rs)
{
	if(dato&(0x8)) GPIOB_PSOR = D7;
	else GPIOB_PCOR = D7;
	
	if(dato&(0x4)) GPIOA_PSOR = D6;
	else GPIOA_PCOR = D6;
	
	if(dato&(0x2)) GPIOD_PSOR = D5;
	else GPIOD_PCOR = D5;
	
	if(dato&(0x1)) GPIOC_PSOR = D4;
	else GPIOC_PCOR = D4;
	
	if(rs == 1)	GPIOE_PSOR = RS;
	else GPIOE_PCOR = RS;
	
}

void enable(void)
{
	GPIOE_PSOR=E;
	delay(20);
	GPIOE_PCOR=E;
}

void delay(unsigned long ms){
	LPTMR0_PSR = 5;
	LPTMR0_CMR = ms-1;
	LPTMR0_CSR=1;
	do{}
	while((LPTMR0_CSR&(1<<7))==0);
	LPTMR0_CSR=(1<<7);
}

void limpiar(void){
	outputPins(0x0,0);
	outputPins(0x1,0);
}

void pasar(void)
{
	outputPins(0xC,0);
	outputPins(0x0,0);
}

void mensaje(unsigned char cadena[16])
{
	unsigned char a;
	unsigned char z;
	//GPIOE_PSOR=RS;
	for(a=0;a<16;a++){
		z=(cadena[a]&0xF0)>>4;
		outputPins(z,1);
		enable();
		z=(cadena[a]&0x0F);
		outputPins(z,1);
		enable();
	}
	//GPIOE_PCOR=RS;
}
