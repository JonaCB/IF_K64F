#include "derivative.h" /* include peripheral declarations */

#define velocity 5243

unsigned int temp;
unsigned int temp1;
unsigned char datorx;
unsigned char first = 0xFF;
unsigned char data_rdy = 0;
unsigned char countrx = 8;

void PORT_init(void);
void Manchester_Tx(unsigned char dato);
unsigned char Manchester_Rx(void);

int main(void)
{	
	PORT_init();
	for(;;);
	
	return 0;
}
//PTC10 SEND
//PTC11 RECEIVE

//FULL TIME 10486
//HALF TIME 5243
void PORT_init(void)
{
	//ENABLE PORT CLOCK (FTM0_CH6 PORT A1)
	SIM_SCGC5 |= (1<<9);
	//ENABLE FTM0 IF CLOCKtxt
	SIM_SCGC6 |= (1<<24);
	//CONFIGURE FTM0_SC TO ENABLE IT WITH SYSTEM CLOCK AND INTERRUPTS
	FTM0_SC |= (1<<3);
	//CONFIGURE FTM_C6SC AS TOGGLE ON MATCH
	FTM0_C6SC = (1<<4)+(1<<2);
	//CONFIGURE FTM_C6SC AS CAPTURE ON RISING OR FALLING EDGE
	FTM0_C7SC = (1<<3)+(1<<2);
	//ENABLE NVIC INTERRUPTS
	//NVICISER1 = (1<<(42%32));
	//CONFIGURE FTM WITH THE COUNT VALUE
	FTM0_C6V = velocity;
}

void Manchester_Tx(unsigned char dato)
{
	unsigned char count = 8;
	do
	{
		if(dato & 0x01) FTM0_C6V+=velocity;			//SWITCH SO IT HAS A MATCH ON ONE CYCLE TIME, INDICATING A 1, IF NOT THEN SWITCHES
		dato>>1;
	}while(--count);
}


unsigned char Manchester_Rx(void)
{
	return datorx;
}


void FTM0_IRQHandler(void)
{
	if(((FTM0_C7SC &(1<<7)) == (1<<7))&&((FTM0_C7SC&(1<<6)) == (1<<6)))
	{
		if(first == 0xFF)
		{
			temp = FTM0_C7V;
			first = ~(first);
		}
		else
		{
			temp1 = FTM0_C7V;
			first = ~first;
		}
		if((temp1-temp)==velocity*2) 
		{
			datorx|0x80;
		}
		datorx>>1;
		countrx--;
		if(!countrx) Manchester_Rx();
	}
}
