#include "derivative.h" /* include peripheral declarations */

unsigned char mens1[]={"AT\r\n"};
unsigned char cmpmens2[]={"OK\r\n"};
unsigned char mens3[]={"AT+CMGF=1\r\n"};
unsigned char mens4[]={"AT+CMGS=\"3339566255\"\r\n"};
unsigned char sms[]={"TEST\r\n"};
unsigned char buffer[80];
unsigned char cmpmens1[]={">"};
unsigned char temp;



void vUART_init(void)
{
	SIM_SCGC4=0x00000C00; //Hab clk UART0 y UART1
	UART0_BDH=0;
	UART0_BDL=11;   //115200 bps
	UART1_BDH=0;
	UART1_BDL=11;    //115200 bps

	UART0_C2=12; // bit 3: Hab Tx, bit 2: Hab Rx
	UART1_C2=12;

	//Pines
	SIM_SCGC5=0x00000C00; //Hab clk PORTB (PB16 y 17 son Rx y Tx) y PORTC
	PORTB_PCR16=0x00000300; //Hab clk PB16 Rx
	PORTB_PCR17=0x00000300; //Hab clk PB17 Tx
	PORTC_PCR3=0x00000300; //Hab clk PC3 Rx
	PORTC_PCR4=0x00000300; //Hab clk PC4 Tx

	SIM_SCGC5|=0x00002400; //Habilitar reloj

}

void vUART_send (unsigned char dato)
{
	do{}while (!(UART1_S1&0x80));
	UART1_D=dato;  
}

void unosnops(void)
{
	unsigned int i = 120;
	while(--i){
		__asm("nop");		//4.7 us min
	};
}

void vUART_send_msg_AT (void)
{
	unsigned char i=0;
	do{
		vUART_send(mens1[i]);
		//i++;
	}while (mens1[++i]!=0);
}

void vUART_send_msg_CMGF (void)
{
	unsigned char i=0;
	do{
		vUART_send(mens3[i]);
		//i++;
	}while (mens3[++i]!=0);
}

void vUART_send_msg_CMGS (void)
{
	unsigned char i=0;
	do{
		vUART_send(mens4[i]);
		//i++;
	}while (mens4[++i]!=0);
}

void vUART_send_SMS (void)
{
	unsigned char i=0;
	do{
		vUART_send(sms[i]);
		//i++;
	}while (sms[++i]!=0);
}

void vUART_send_CTRLZ (void)
{
		vUART_send(0x1A);	
}

unsigned char u8parser1 (void)  //">"
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


int main(void)
{
	vUART_init();
	
	vUART_send_msg_AT();
	unosnops();
	vUART_send_msg_CMGF();
	unosnops();
	vUART_send_msg_CMGS();
	unosnops();
	vUART_send_SMS();
	unosnops();
	vUART_send_CTRLZ();
	/*
	
	//if(u8parser1()==1){
	delay();
	vUART_send_msg_AT();
	if (u8parser2()==1) //OK
	{	
		delay();
		vUART_send_msg_CMGF();
		if (u8parser2()==1) //OK
		{	
			delay();
			vUART_send_msg_CMGS();
			if (u8parser1()==1) //>
			{
				delay();
				vUART_send_SMS();
				vUART_send_CTRLZ();
				if (u8parser2()==1) //OK
				{	
					return 1;
				}
			}
		}
	}

	*/
	return 0;
}
