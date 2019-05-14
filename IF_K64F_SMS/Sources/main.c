#include "derivative.h" /* include peripheral declarations */

//DEFINES
#define LLAMAJONA "ATD7221755140;\r\n"
#define CUELGAJONA "ATH\r\n"
#define GOTMSG "+CMTI:"
#define READMSG "+CMGR:"
#define UARTSEND UART3_C2 |= (1<<6)+(1<<3);

//POINTERS FOR TX AND RX
unsigned char *ptr;
unsigned char *rx_ptr;
unsigned char *msg_ptr;
unsigned char *aux_ptr;
unsigned char flg = 0;		//bit 0 = unrecognized chain, bit 1 = about to rx the msg number
unsigned char aux_flg = 0b00000011;

//ARRAYS
unsigned char sendinfo[2][18] = {LLAMAJONA, CUELGAJONA};
unsigned char rx_info[2][9] = {GOTMSG,READMSG};
unsigned char aux_array[2];
unsigned char rx_buffer[50];
unsigned char msg_buffer[100];
unsigned char read_msg[14] = {"AT+CMGR=0  \r\n"};
unsigned char send_msg[4][25] = {"AT+CMGF=1\r\n","AT+CMGS=\"7221755140\"\r\n","Hello world\x1A"};//,"\x1A"};
//CHECAR COMO MANDAR EL ASCII

//ITERATORS FOR ARRAY USED FOR PARSING
unsigned char i;
unsigned char j;
unsigned char ptr_pos = 0;
unsigned volatile char vld_opt = 0;

unsigned char tmp;

// FUNCTION HEADERS
void delay(unsigned long tiempo_ms);
void UART_init(void);


int main(void)
{
	UART_init();
	rx_ptr = rx_buffer;
	msg_ptr = msg_buffer; 
	/*ptr = &sendinfo[0][0];
	UARTSEND
	delay(10000);
	ptr = &sendinfo[1][0];
	UARTSEND
	delay(1000);*/
	
	for(;;)
	{
		if(flg&(0x4))
		{
			ptr = read_msg;
			UARTSEND
			flg&=~(1<<2);
			vld_opt = 0;
		}
		if(flg&(0x40))
		{
			ptr = &send_msg[0][0];
			UARTSEND
			delay(1000);
			ptr = &send_msg[1][0];
			UARTSEND
			delay(1000);
			ptr = &send_msg[2][0];
			UARTSEND
			delay(1000);
			ptr = &send_msg[3][0];
			UARTSEND
			
			delay(100);
			flg&=~(1<<6);
			flg|=(1<<7);
		}
	}
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
	//UART3_C2 |= (1<<5);		//RXIEN if RDRF
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
		if(!vld_opt)
		{
			//ITERATOR OF AUXILIARY ARRAY
			for(i = 0; i < 2; i++)
			{
				if(!(aux_flg&(1<<i)))
				{
					flg |= 1;
				}
				if(!(flg&(0x1)))
				{
					if(UART3_D != rx_info[i][ptr_pos])
					{
						aux_flg&=~(1<<i);
					}
				}
				flg &= ~(1<<0);
			}
			ptr_pos++;
			if(aux_flg == 1) 
			{
				vld_opt = 1;
				aux_ptr = &read_msg[8];
				aux_flg = 3;
				ptr_pos = 0;
			}
			else if (aux_flg == 2)
			{
				vld_opt = 2;
				aux_flg = 3;
				ptr_pos = 0;
			}
			if(!aux_flg)
			{
				aux_flg = 3;
				ptr_pos = 0;
			}
			
			//*rx_ptr = UART3_D;
			//UART3_D = *rx_ptr;
			(void) UART3_S1;
		}
		else if(flg&(0x8))
		{
			if(flg&(0x20))
			{
				*msg_ptr = UART3_D;
				msg_ptr++;
			}
			if(flg&(0x20) && UART3_D == '\n'){
				flg = 0;
				flg = (1<<6);
				vld_opt = 0;
			}
			if(flg&(0x10) && UART3_D == '\n')
			{
				flg|=(1<<5);
			}
			if(UART3_D == '\n')	flg|=(1<<4);
		}
		else if(vld_opt == 1)
		{
			tmp = UART3_D;
			if(flg&(0x2))
			{
				*aux_ptr = UART3_D;
				if((aux_ptr == &read_msg[10])||UART3_D == '\n')
				{
					flg &= ~(1<<1);
					flg |= (1<<2);
				}
				aux_ptr++;
			}
			if(tmp == ',')
			{
				flg |= (1<<1);
			}
		}
		else if(vld_opt == 2)
		{
			if(tmp >= 8) 
			{
				flg|=(1<<3);
				tmp = 0;
			}
			if(UART3_D == '"')
			{
				tmp++;
			}
		}
	}
	if(UART3_S1 &(0x40))
	{
		(void) UART3_S1;
		UART3_D = *ptr++;
		if(*ptr == 0)
		{
			UART3_C2 &= ~(1<<6);
			UART3_C2 &= ~(1<<3);
			//UART3_D = 0;
			ptr = &sendinfo[0][0];
		}
	}
}

void delay(unsigned long tiempo_ms)
{
	LPTMR0_PSR|=0b00000101;	//Prescaler bypass y LPO de CLK
	LPTMR0_CMR=tiempo_ms;	//Asignar tiempo_ms a registro con valor a comparar
	LPTMR0_CSR|=0b00000001;	//Timer Enable 
	do{}
	while(!(LPTMR0_CSR&0b10000000));				//Esperar la bandera TCF
	LPTMR0_CSR |= (1<<7);
}
