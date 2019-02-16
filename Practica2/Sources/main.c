#include "derivative.h" // include peripheral declarations 

void delay (unsigned long tiempo_ms);

void init(){
	// Inicializar reloj
		SIM_SCGC5= 0b0000001100000000001;
	// Seleccionar como GPIO los leds
		PORTD_PCR1=0x00000100;
		PORTD_PCR2=0x00000100;
		PORTD_PCR3=0x00000100;
		PORTD_PCR0=0x00000100;

	// Seleccionar switch como GPIO
		PORTC_PCR1=0x00000100;
		
		GPIOD_PDOR = 0;
	// Seleccionar leds como salida
		GPIOD_PDDR=0x0000000F; //0b00001111
	// Seleccionar switch como entrada
		GPIOC_PDDR=0x00000000;
}

int main(void)
{
	init();	
	unsigned long secuencia1[]={0b00001000,0b00000001};//8421 14,7,0b00001110,0b00000111
	unsigned long secuencia2[]={0b00000100,0b00000010}; // 11,13   ,0b00000010,,0b00000100

	unsigned int i=0;
	
	for(;;) {	 
		if(GPIOC_PDIR == 0x00000000)
		{	
			GPIOD_PDOR=secuencia1[i++%2];
		}
		else
		{	
			GPIOD_PDOR=secuencia2[i--%2];				
		}
		delay(3);		
	};
	
	return 0;
}


void delay (unsigned long tiempo_ms){
	LPTMR0_PSR=0b00000101;
	LPTMR0_CMR = tiempo_ms;//999
	LPTMR0_CSR=0b00000001; // timer enable
	do{}
	while ((LPTMR0_CSR&0b10000000)==0);
	LPTMR0_CSR=(1<<7);	
}
