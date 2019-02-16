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

//PORTC10 = SDA
//PORTC11 = SCL

void unosnops(void);
unsigned char vIIC_rec_byte(void);
void vIIC_init(void);
void vIIC_start(void);
void vIIC_ack_out (unsigned char ack_bit);
void vIIC_send_byte(unsigned char dato);
unsigned char vIIC_ack_input(void);
void read_hour(unsigned char* lectura);//(unsigned char start_dir, unsigned char* datos, uint8_t num_datos);
void set_hour(unsigned char* dato);//(unsigned char start_dir, unsigned char* dato, uint8_t num_datos);
unsigned char parser(unsigned char dato);

int main(void)
{	//HORA CON 20:11
	unsigned char hora[3]={
         0x00,
         0x11,
         0x20, // bit6=0 (24hr),  bit5=0, bit4=1, 
    };
	unsigned char lectura[3];
	vIIC_init();
	set_hour(hora);
	read_hour(lectura);
	
	for(;;);
	
	return 0;
}

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
		if(GPIOC_PDIR&=(1<<10))temp++;
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

void set_hour(unsigned char* dato)
{
	unsigned char datanum = 3;
	int i;
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR);
	if(vIIC_ack_input()==0)
	{
			vIIC_send_byte(SEC_ADDR);
			if(vIIC_ack_input()==0)
			{
				for(i=0; i<datanum;i++)
				{
					vIIC_send_byte(dato[i]);
				}
			}
			vIIC_stop();
	}
}

void read_hour(unsigned char* lectura)
{
	int i;
	unsigned char datanum = 3;
	vIIC_start();
	vIIC_send_byte(SLAVE_ADDR);
	if(vIIC_ack_input()==0)
	{
			vIIC_send_byte(SEC_ADDR);
			if(vIIC_ack_input()==0)
			{
				//RESTART TRANSACTION ONCE RTC IS POINTED 
				vIIC_stop();
				vIIC_start();
				vIIC_send_byte(SLAVE_ADDR|1);
				vIIC_ack_out(0);
				for(i=0; i<datanum;i++)
				{
					lectura[i]=vIIC_rec_byte();
					vIIC_ack_out(0);
				}
			}
			vIIC_stop();
	}
}
/*
unsigned char parser_read(unsigned char dato)
{
	unsigned char low = dato&0xF0;
	unsigned char high = dato&0x0F;
	
}

unsigned char parser_write(unsigned char dato)
{
	unsigned char temp = 0;
	unsigned char cnt = 6;
	unsigned char unit = 0;
	unsigned char tens = 0;
	do
	{
		if(dato&0x01) temp +=1<<6-cnt;
	}while(--cnt);
	unit = temp%10;
	tens = temp/10;
	
}*/
