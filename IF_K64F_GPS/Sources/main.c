#include "derivative.h" /* include peripheral declarations */
//Testing
unsigned char header[]={"$GPRMC,"};				//THIS IS THE STRING TO CMP TO KNOW IF IT'S A VALID DATA
unsigned char *ptr;								//ITERATOR TO KEEP TRACK OF WHERE IT IS
unsigned char buffer[80];
unsigned char temp;
unsigned char hour[6];
unsigned char north[9];

unsigned int *address[] = {*temp, *valid};		//ARRAY OF POINTERS REFERENCING THE ARRAYS WHERE IT´S GONNA BE DUMPED

//HEADERS
void vUART_init(void);
void vUART_send (unsigned char dato);
void delay (void);
unsigned char u8UART_receive (void);
void vUART_receive_buffer (void);
void vUART_send_msg_mux (void);
unsigned char u8parser1 (void);

int main(void)
{
	vUART_init();
	
	//if(u8parser1()==1){
	vUART_send_msg_CIFSR();
	if (u8parser2()==1)
	{	
		IP_copy();
		vUART_send_msg_mux();
		if (u8parser2()==1)
		{	
			vUART_send_msg_Server();
			if (u8parser2()==1)
			{
				//completa_mensaje();
			do{	
				if (u8parser3()==1)
				{	
					//LED=u8UART_receive();
					if (LED=='R')
					{
						GPIOB_PDDR=0x00400000;
						GPIOE_PDDR=0x00000000;
					}
					if(LED=='G')
					{
						GPIOE_PDDR=0x04000000;
						GPIOB_PDDR=0x00000000;
						
					}
					if(LED=='B')
					{
						GPIOB_PDDR=0x00200000;
						GPIOE_PDDR=0x00000000;
					}
					if(LED=='S')
					{
						GPIOB_PDDR=0x00000000;
						GPIOE_PDDR=0x00000000;
						
					}
					if(LED=='P')
					{
						GPIOB_PDDR=(0x00200000|0x00400000);
						GPIOE_PDDR=0x00000000;
					}
					if(LED=='T')
					{
						GPIOB_PDDR=0x00200000;
						GPIOE_PDDR=0x04000000;
					}
					if(LED=='Y')
					{
						GPIOB_PDDR=0x00400000;
						GPIOE_PDDR=0x04000000;
					}
					if(LED=='W')
					{
						GPIOB_PDDR=(0x00200000|0x00400000);
						GPIOE_PDDR=0x04000000;
					}
				}
			} while (1);
			}
		}
	//}
	}

	for(;;);
	return 0;
}

void vUART_init(void)
{
	SIM_SCGC4=0x00000C00; 			//ENABLE UART0 & UART1 CLK (ONLY NEED UART0)
	UART0_BDH=0;
	UART0_BDL=11;   				//115200 bps	CHECK IF WE NEED 9600 BPS

	UART0_C2=(1<<3)+(1<<2); 					//B3:ENABLE TX 2: ENABLE RX

	//PINS
	SIM_SCGC5=0x00000C00; 			//ENABLE CLK PORTB (PB16 y 17 son Rx y Tx) y PORTC
	PORTB_PCR16=0x00000300; 		//Hab clk PB16 Rx
	PORTB_PCR17=0x00000300; 		//Hab clk PB17 Tx
	PORTC_PCR3=0x00000300; 			//Hab clk PC3 Rx
	PORTC_PCR4=0x00000300; 			//Hab clk PC4 Tx

}

//CONVERT FROM POLLING TO INTERRUPT
void vUART_send (unsigned char dato)
{
	do{}while (!(UART1_S1&0x80));
	UART1_D=dato;  
}

//CHANGE TO UNOSNOPS
void delay (void)
{
unsigned long i;
	for (i=0;i<=200000;i++);
}

//CHANGE TO USE INTERRUPTS
unsigned char u8UART_receive (void)
{
	do{}while (!(UART1_S1&0x20));

	do{}while (!(UART0_S1&0x80));  //Eco a la PC
	UART0_D=UART1_D;

	return UART1_D;
}

//CHANGE TO USE INTERRUPTS
void vUART_receive_buffer (void)
{
	unsigned char i=0;
	unsigned long cont=0;
	do{
		buffer[i]=u8UART_receive();
	}while ((i<80)&& (++cont<=10000000));
}

void vUART_send_msg_mux (void)
{
	unsigned char i=0;
	do{
		vUART_send(mens3[i]);
		//i++;
	}while (mens3[++i]!=0);
}

unsigned char u8parser1 (void)  //"READY"
{
  unsigned char i=0;
  //unsigned char j=0;
  unsigned long cont;
  unsigned char temp;
  do{
    cont=0;
    do{}while ((!(UART1_S1&0x20))&& (++cont<=1000000));
    
    if (cont!=1000000)
    {
    	temp=UART1_D;
    	UART0_D=temp;
     
      if (cmpmens1[i]==temp) i++;
      else i=0;
    }
  }while (cmpmens1[i]!=0);
  if (cont==1000000) return 0;
  else return 1;
}

unsigned char u8parser3 (void)  
{
	unsigned char i=0;    
	unsigned long cont;
	do{
		cont=0;
		do{}while ((!(UART1_S1&0x20))&& (++cont<=1000000));
		if (cont!=1000000)
		{
			
			if (equipo[i]==UART1_D) i++;
				/*if(i == 8){
					LED = UART1_D;
					i++;
				}*/
			else i=0;
		}
	}while ((equipo[i]!=0)&&(!(UART1_S1&0x20)));
	if (cont==1000000) return 0;
	else{
	do{}while(!(UART1_S1&0x20));
		LED=UART1_D;
		UART0_D=LED;
		return 1;
	}
}

unsigned char u8parser2 (void)  //"OK"
{
	unsigned char i=0;
	unsigned char j=0;
	unsigned long cont;
	do{
		cont=0;
		do{}while ((!(UART1_S1&0x20))&& (++cont<=10000000));
		if (cont!=10000000)
		{   temp=UART1_D; 
			UART0_D=temp;
			
			if(cmpmens2[i]==temp){
				i++;
			}
			else{
				i=0;
			}
			buffer[j++]=UART1_D;
		if (cmpmens2[i]==UART1_D) i++;
		else i=0;
		}
	}while ((cmpmens2[i]!=0)&&(++cont<=10000000));
	if (cont==1000000) return 0;
	else return 1;
}
