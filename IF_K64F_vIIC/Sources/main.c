#include "derivative.h" /* include peripheral declarations */

#define SDA_INPUT GPIOC_PDDR &= 0xFFFFFBFF;
#define SDA_OUTPUT GPIOC_PDDR |= (1<<10);
#define SDADIS GPIOC_PCOR |= (1<<10);
#define SDAEN GPIOC_PSOR |= (1<<10);
#define SCLDIS GPIOC_PCOR |= (1<<11);
#define SCLEN GPIOC_PSOR |= (1<<11);

//DEFINES DEL RTC
#define SLAVE_ADDR 0xD0
#define SEC_ADDR 0x00
#define MIN_ADDR 0x01
#define HR_ADDR 0x02
#define DAY_ADDR 0x3
#define DATE_ADDR 0x04
#define MONTH_ADDR 0x05
#define YEAR_ADDR 0x06

//PORTC10 = SDA
//PORTC11 = SCL

void unosnops(void);
unsigned char vIIC_rec_byte(void);
void vIIC_init(void);
void vIIC_start(void);
void vIIC_ack_out (unsigned char ack_bit);
void vIIC_send_byte(unsigned char dato);
unsigned char vIIC_ack_input(void);
void vIIC_byte_write(unsigned char direccion, unsigned char dato);
unsigned char vIIC_read_byte(unsigned char direccion);
unsigned char vIIC_page_read(unsigned char start_dir, unsigned char* datos, uint8_t num_datos);
unsigned char vIIC_page_write(unsigned char start_dir, unsigned char* dato, uint8_t num_datos);

int main(void)
{	//HORA CON 2:08 pm
	unsigned char fecha[7]={
         0x00, 
         0x32,
         0x16, // bit6=0 (24hr),  bit5=0, bit4=1, 
         0x06, //dia de la semana
         0x09,//dia del mes
         0x02,//mes
         0x19//año
    };
	unsigned char fecha_lect[7];
	//unsigned char lect;
	vIIC_init();
	vIIC_page_write(SEC_ADDR,fecha,8);
	//vIIC_byte_write(0x02,0x15);
	//lect = vIIC_read_byte(0x02);
	vIIC_page_read(SEC_ADDR, fecha_lect,8);
	
	
	for(;;);
	
	return 0;
}

/*
void UART_init()
{
	//ENABLE UART INTERFACE CLOCK
	SIM_SCGC4 |= (1<<10);
	
	//CONFIGURE BAUDRATE
	UART0_BDH=0;		  
	UART0_BDL=137;
	
	//ENABLE RX AND TX
	UART0_C2=0x0C;
	
	SIM_SCGC5|=0x00000400;
	//PORT B16 ENABLED AS RX
	PORTB_PCR16=0x00000300;
	//PORT B17 ENABLED AS TX
	PORTB_PCR17=0x00000300;
	
	//ENABLE INTERRUPTS
	NVICICER0|= (1<<(31%32));
	NVICISER0|= (1<<(31%32));
}
*/

void unosnops(void)
{
	unsigned int i = 120;
	while(--i){
		__asm("nop");		//4.7 us min
	};
}



void vIIC_init(void)
{
	//Define pin for SCL, output, 0
	//Define pin for SDA, output, 0
	SIM_SCGC5 = (1<<11);			//PORTC CLOCK ENABLED
	PORTC_PCR10 = 0x00000100;		//PTC10 AS GPIO
	PORTC_PCR11 = 0x00000100;		//PTC11 AS GPIO
	SDA_OUTPUT
	GPIOC_PDDR |= (1<<11);			//PORTS CONFIGURED AS OUTPUT
	GPIOC_PCOR |= (1<<11)+(1<<10);	//PORTS DEFINED AS 0
}

void vIIC_start(void)
{
	//SDA = 1 & SCL = 1
	SDAEN
	SCLEN
	//SDA 1 SCL 1
	unosnops();
	//SDA = 0
	SDADIS
	//SDA 0 SCL 1
	unosnops();
	//SCL = 0;
	SCLDIS
	unosnops();
	//SCL = 0 & SDA = 0
	
}

void vIIC_stop(void)
{
	//SDA = 0;
	SDADIS
	//SCL = 1;
	SCLEN
	//esperar t "unos nops" asm("nop");
	unosnops();
	//SDA = 1;
	SDAEN
	//esperar t12 "unos nops" asm("nop");
	unosnops();
	//SCL = 0;
	SCLDIS
	//SDA = 0;
	SDADIS
}

unsigned char vIIC_rec_byte(void)
{
	unsigned char cont = 8;
	unsigned char temp = 0;
	//SDA input
	SDA_INPUT
	do
	{
		//SCL=1;
		SCLEN
		unosnops();
		temp<<=1;
		if(GPIOC_PDIR&(1<<10))temp++;
		SCLDIS
		unosnops();
	}while(--cont);
	//SDA output
	SDA_OUTPUT
	return temp;
}

void vIIC_ack_out (unsigned char ack_bit)
{
	if(ack_bit) SDAEN;
	SCLEN;
	//ESPERAR
	unosnops();
	SCLDIS
	SDADIS		
}

void vIIC_send_byte(unsigned char dato)
{
	unsigned char cont = 8;
	do
	{
		if(dato&0x80) SDAEN //SDA = 1
		else SDADIS //SDA = 0;
		//SCL=1
		SCLEN
		//esperar
		unosnops();
		//SCL = 0
		SCLDIS
		//esperar
		unosnops();
		dato<<=1;
	}while(--cont);
	//SDA = 0;
	SDADIS
}

unsigned char vIIC_ack_input(void)
{
	unsigned char temp = 0;
	//SDA entrada
	SDA_INPUT
	//SCL = 1
	SCLEN
	if(GPIOC_PDIR&(0x400)) temp = 1;
	//SCL = 0;
	SCLDIS
	//SDA salida
	SDA_OUTPUT
	return temp;
}

/*
void vIIC_byte_write(unsigned short direccion, unsigned char dato)
{
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR);
	if(vIIC_ack_input()==0)
	{
		vIIC_send_byte(direccion>>8);
		if(vIIC_ack_input()==0)
		{
			vIIC_send_byte(direccion);
			if(vIIC_ack_input()==0)
			{
				vIIC_send_byte(dato);
				if(vIIC_ack_input()==0)
				{
					vIIC_stop();
					//CHANGES TESTING
				}
			}
		}
		
	}
}
*/

void vIIC_byte_write(unsigned char direccion, unsigned char dato)
{
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR);
	if(vIIC_ack_input()==0)
	{
		vIIC_send_byte(direccion);
		if(vIIC_ack_input()==0)
		{
			vIIC_send_byte(dato);
			if(vIIC_ack_input()==0)
			{
				vIIC_stop();
				//CHANGES TESTING
			}
		}
	}
}

unsigned char vIIC_read_byte(unsigned char direccion)
{
	unsigned char temp = 0xFF;
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR); //ENVIAS PRIMER DATO PARA COLOCAR EL POINTER DEL RTC
	if(vIIC_ack_input()==0)
	{
		vIIC_send_byte(direccion);
		if(vIIC_ack_input()==0)
		{ 
			//Una vez modificado el pointer del RTC, reinicias la transaccion pero para lectura 
			vIIC_stop();
			vIIC_start();
			vIIC_send_byte(SLAVE_ADDR+1);
			vIIC_ack_out(0);
			temp = vIIC_rec_byte();
			vIIC_ack_out(1);
			vIIC_stop();
		}	
	}
	return temp;
}

unsigned char vIIC_page_write(unsigned char start_dir, unsigned char* dato, uint8_t num_datos)
{
	unsigned char ack_flag=0;
	int i;
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR);
	if(vIIC_ack_input()==0)
	{
			vIIC_send_byte(start_dir);
			if(vIIC_ack_input()==0)
			{
				for(i=0; i<num_datos;i++)
				{
					vIIC_send_byte(dato[i]);
					if(vIIC_ack_input()==1)
						return ack_flag=1;
				}
			}
			else return ack_flag=2;
			vIIC_stop();
	}
	else return ack_flag=3;
return ack_flag;
}

unsigned char vIIC_page_read(unsigned char start_dir, unsigned char* datos, uint8_t num_datos)
{
	unsigned char ack_flag=0;
	int i;
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR);
	if(vIIC_ack_input()==0)
	{
			vIIC_send_byte(start_dir);
			if(vIIC_ack_input()==0)
			{
				//Una vez modificado el pointer del RTC, reinicias la transaccion pero para lectura 
				vIIC_stop();
				vIIC_start();
				vIIC_send_byte(SLAVE_ADDR|1);
				vIIC_ack_out(1);
				for(i=0; i<num_datos;i++)
				{
					datos[i]=vIIC_rec_byte();
					vIIC_ack_out(0);
				}
			}
			else return ack_flag=2;
			vIIC_stop();
	}
	else return ack_flag=3;
return ack_flag;
}
