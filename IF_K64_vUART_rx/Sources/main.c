#include "derivative.h" /* include peripheral declarations */

unsigned char dato;
void interrupt_IC_OC(void)
{
	//apagar bandera
	//Revisar si es IC o OC
	if(IC)
	{
		//Cambiar func a OC sin manejo de pin
		//valor_referencia = IC_value + tb+tb/2;
		cont = 10;			//en la 10ma se avisa que ya terminó de mandar el bit
	}
	else
	{
		if(cont>2)
		{
			dato>>1;
			if(RxD==1) dato|=0x80;
			cont --;
			//valor_referencia += tb;
		}
		if(cont == 2)
		{
			if(RxD == 0)	//frame error
			else cont --;
		}
		if(cont == 1)
		{
			//Data ready = 1;
			//Cfg IC flanco de bajada
		}
	}
}

void vUART_Rxinit(void)
{
	//Configurar input capture: pin,timer,input capture
}


int main(void)
{
	int counter = 0;
	
	
	
	
	for(;;) {	   
	   	counter++;
	}
	
	return 0;
}
