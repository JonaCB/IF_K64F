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
void send_uart (unsigned char dato);
void delay (void);
unsigned char u8UART_receive (void);
void vUART_receive_buffer (void);

//void vUART_send_msg_mux (void);
//unsigned char u8parser1 (void);


/*
 FALTA: configurcion de UART1, cambiar a interrupciones mezclar con location_parse, 
 */
int main(void)
{
	vUART_init();
	
	//if(u8parser1()==1){
	vUART_send_msg_CIFSR();
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
void send_uart (unsigned char dato)
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
	do{}while (!(UART0_S1&0x20)); //poll for receive buffer

	//do{}while (!(UART0_S1&0x80));  //Eco a la PC
	//UART0_D=UART1_D;

	return UART0_D;
}

//CHANGE TO USE INTERRUPTS
void vUART_receive_buffer(void)
{
	unsigned char i=0;
	unsigned long cont=0;
	do{
		buffer[i]=u8UART_receive();
	}while ((i<80)&& (++cont<=10000000));
}


unsigned char location_parse (void)  //"READY"
{
	uint8_t comas_counter=0; 
	uint8_t loc_buffer[20];
	uint8_t i=0, dato;
	
	do {}while(u8UART_receive!=0x24); //wait $
	do {}while(u8UART_receive!=0x47);//wait G
	do {}while(u8UART_receive!=0x50); //wait P
	do {}while(u8UART_receive!=0x52);//wait R
	do {}while(u8UART_receive!=0x40); //WAIT M
	do {}while(u8UART_receive!=0x43); //wait C
	
	//despues de recibir $GPRMC cuenta 3 comas
	do{
		if(u8UART_receive==0x2C)
			++comas_counter;
	} while(comas_counter<=3);
	
	for(comas_counter=0; comas_counter<4; i++)
	{
		dato=u8UART_receive;
		if(dato==0x2C)
			comas_counter++;
		else
			loc_buffer[i]=dato;
	}
	parse_ubiNS(loc_buffer);
	parse_ubiEW(&loc_buffer[8]);
}

void parse_ubiNS(uint8_t *buffer)
{
	uint8_t cont; 
	for(cont=0;cont<7;cont++)
	{
		if(cont==2){
			send_uart(0xA7) //DEGREE SYMBOL
			send_uart(buffer[cont])
		}
		else if(cont==4)
			send_uart(0x27) //send ´
		else
			send_uart(buffer[cont])
	}
	send_uart(0x22) //send "
	send_uart(buffer[cont]);
}

void parse_ubiEW(uint8_t *buffer)
{
	uint8_t cont; 
	for(cont=0;cont<8;cont++)
	{
		if(cont==2){
			send_uart(0xA7) //DEGREE SYMBOL
			send_uart(buffer[cont])
		}
		else if(cont==5)
			send_uart(0x27) //send ´
		else
			send_uart(buffer[cont])
	}
	send_uart(0x22) //send "
	send_uart(buffer[cont]);
}

