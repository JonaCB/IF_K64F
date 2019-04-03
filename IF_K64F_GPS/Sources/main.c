#include "derivative.h" /* include peripheral declarations */

#define UARTSEND UART3_C2 |= (1<<6)+(1<<3);
#define UARTDIS UART3_C2 &= ~((1<<6)+(1<<3));
#define GPS_HDR "$GPRMC"
#define GPS_INFO "$CGNSINF"
#define START_GPS "AT+CGNSINF\r\n"
#define SPAM_GPS "AT+CGNSTST=1\r\n" 


unsigned char *ptr_pos = 0;
unsigned char *msg_ptr;
//FLAGS AND STATUS CONTROL
unsigned char flg = 0b00000011;
unsigned char aux_flg = 0;
unsigned char vld_opt = 0;
unsigned char msg_buff[70];

//ARRAYS
unsigned char gps_info[2][10] = {GPS_HDR, GPS_INFO};

//ITERATORS
unsigned char i = 0;

//HEADERS
void UART_init(void);

int main(void)
{	
	UART_init();
	msg_ptr = msg_buff;
	
	for(;;);
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
	UART3_RWFIFO = 0x01;
	UART3_C2 = (1<<5)+0x0C;
	UART3_D = 0;
	UART3_C2 |= 0x40;
	
	//PORT B16 ENABLED AS RX
	PORTC_PCR16=0x00000300;
	//PORT B17 ENABLED AS TX
	PORTC_PCR17=0x00000300;
	
	//ENABLE INTERRUPTS
	NVICICER1|= (1<<(37%32));
	NVICISER1|= (1<<(37%32));
}

void UART3_Status_IRQHandler(void)
{
	if(UART3_S1 & (0x20))
	{
		(void) UART3_S1;
		if(!vld_opt)
		{
			for(i = 0; i < 2; i++)
			{
				if(!(aux_flg&(1<<i)))
				{
					flg |= 1;
				}
				if(!(flg&(0x1)))
				{
					if(UART3_D != gps_info[i][ptr_pos])
					{
						aux_flg&=~(1<<i);
					}
				}
				flg &= ~(1<<0);
			}
			ptr_pos++;
			if(!aux_flg)
			{
				ptr_pos = 0;
				flg = 3;
			}
			else if(aux_flg == 1) 
			{
				vld_opt = 1;
				ptr_pos = 0;
				flg = 3;
			}
			else if(aux_flg == 2)
			{
				vld_opt = 2;
				ptr_pos = 0;
				flg = 3;
			}
		}
		if(vld_opt == 1)
		{
			if(UART3_D == 0)vld_opt = 0;
			*msg_ptr = UART3_D;
			msg_ptr++;
		}
	}
	if(UART3_S1 &(0x40))
	{
		(void) UART3_S1;
		UART3_D = *ptr++;
		if(*ptr == 0)
		{
			UARTDIS
		}
	}
}
