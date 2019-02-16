#include "derivative.h" /* include peripheral declarations */

/* 1.habilitar el clk del interfaz. para PORTx, timers, adc, uart, i2c
   2.Seleccionar MUX la funcion del pin. debe ser GPIO
   3.Ponerla como entrada '0' o si es salida '1'.
   4.Escribir pin
*/

int var_a32 = 0;

void delay (unsigned long tiempo_ms){
	LPTMR0_PSR=0b00000101;
	LPTMR0_CMR = tiempo_ms;//999
	LPTMR0_CSR=0b00000001; // timer enable
	do{}
	while ((LPTMR0_CSR&0b10000000)==0);
	LPTMR0_CSR=(1<<7);	
}

int main(void){
	unsigned int i=0;
	unsigned long v1=0b1110;
	unsigned long v2=0b1101;
	unsigned long v3=0b1011;
	unsigned long v4=0b0111;

	unsigned long secuencia[4] = {v1,v2,v3,v4};
	/*SIM_SCGC5 = (1<<12)+1; //Clk PORTB = 1
	SIM_SCGC5 = (1<<11);
	PORTD_PCR0 = 1<<8; //Alternativa 1, PTE0 PCRn n=numero de pin  
	PORTD_PCR1 = 1<<8; //Alternativa 1, PTE1
	PORTD_PCR2 = 1<<8; //Alternativa 1, PTE2
	PORTD_PCR3 = 1<<8; //Alternativa 1, PTE3
	PORTC_PCR4 = 1<<8;
	
	GPIOD_PDDR = 15; //Configurar esos puertos como output, poniendo 1 en bits 3,2,1 y 0
	GPIOC_PDDR = 0;*/
	
	SIM_SCGC5 = 6145; //Activar puerto C y D
	PORTD_PCR0 = 256; //habilitar PD0
	PORTD_PCR1 = 256; //habilitar PD1
	PORTD_PCR3 = 256; //habilitar PD2
	PORTD_PCR2 = 256; //habilitar PD3
	PORTC_PCR12 = 256; //habilitar pc12
	PORTC_PCR4 = 256;	//habilitar pc4
	GPIOD_PDDR = 15; //Pines las 4 salidas  
	GPIOC_PDDR = 0; //Entradas	
	
	for(;;){
		if(GPIOC_PDIR == 0) { 
			GPIOD_PDOR = secuencia[(i++)%4];
		}else{
			GPIOD_PDOR = secuencia[(i--)%4];	
		}
		delay(50);
	}
}
