#include "derivative.h" /* include peripheral declarations */
//MASKS TO USE I2C
#define I2C_RXMASK 0xF7
#define I2C_START I2C_C1_MST_MASK
#define I2C_TX I2C_C1_TX_MASK
//RTC DEFINES
#define SLAVE_ADDR 0xD0
#define SEC_ADDR 0x00
#define MIN_ADDR 0x01
#define HR_ADDR 0x02
#define DAY_ADDR 0x3
#define DATE_ADDR 0x04
#define MONTH_ADDR 0x05
#define YEAR_ADDR 0x06

//PTE24 I2C_SCL
//PTE25 I2C_SDA

//I2C_D IF TRANSMIT ENABLED, DATA TO BE TRANSMITTED. IF RECEIVE, READING STARTS THE NEXT RECEPTION OF BYTE
void I2C_init(void);
void unosnops(uint8_t cnt);
uint8_t I2C_byterx(uint8_t addr);
void I2C_bytetx(uint8_t data, uint8_t addr);
void I2C_wait(void);

int main(void)
{
//	unsigned volatile char m = 0;
	unsigned char data;
	
	I2C_init();
	
	I2C_bytetx(0x10, MIN_ADDR);
	//m = I2C_byterx(MIN_ADDR);
	
	for(;;)
	{
		I2C_bytetx(0x10, MIN_ADDR);
		data = I2C_byterx(MIN_ADDR);
		unosnops(50000);
	}
	return 0;
}

void I2C_init(void)
{
	//CLOCKING AND PORT INITIALIZATION
	SIM_SCGC4 |= (1<<6);					//ENABLE I2C CLOCK
	SIM_SCGC5 |= (1<<13);					//ENABLE PORT CLOCK
	PORTE_PCR24 |= (1<<10)+(1<<8);			//ENABLE PORT AS I2C_SCL
	PORTE_PCR25 |= (1<<10)+(1<<8);			//ENABLE PORT AS I2C_SDA
	
	//I2C CONFIGURATION
	I2C0_F = 0x56;							//SDA TIME 1.125us, START 4.125us, STOP 5.750us
	I2C0_C1 |= (1<<7);						//ENABLE I2C MODULE
	
	
	//INTERRUPT INITIALIZATION
	//NVICISER0|=(1<<(24%32));
	//NVICICER0|=(1<<(24%32));
}

void unosnops(uint8_t cnt)
{
	int i;
	for(i=0;i<cnt;i++)
	{
		__asm("nop");
	}
}

void I2C_bytetx(uint8_t data, uint8_t addr)
{
	I2C0_C1&=~(1<<3);						//DISABLE ACK TRANSMISSION
	I2C0_C1|=I2C_TX;						//ENABLE AS TRANSMITTER
	I2C0_C1|=I2C_START;						//SEND START BIT
	
	I2C0_D = SLAVE_ADDR;					//SEND SLAVE ADDRESS
	I2C_wait();								//WAIT ACK
	
	I2C0_D = addr;							//SEND REGISTER ADDR
	I2C_wait();								//WAIT ACK
	
	I2C0_D = data;							//SEND DATA
	I2C_wait();								//WAIT ACK
	
	I2C0_C1&=~I2C_START;					//SEND STOP BIT
	I2C0_C1&=~I2C_TX;						//SET I2C AS TX
	
	
	unosnops(200);
	
}

uint8_t I2C_byterx(uint8_t addr)
{
	uint8_t data=0;
	I2C0_C1|=I2C_TX;						//SET I2C AS TX
	I2C0_C1|=I2C_START;						//SEND START BIT

	I2C0_D = SLAVE_ADDR;					//SEND SLAVE ADDRESS
	I2C_wait();								//WAIT ACK
	
	I2C0_D = addr;							//SEND REGISTER ADDR
	I2C_wait();								//WAIT ACK
	
	I2C0_C1|=(1<<2);						//REPEAT START
	I2C0_D = SLAVE_ADDR+1;					//SEND SLAVE ADDRESS
	I2C_wait();								//WAIT ACK (Check whether to wait ack or send ack 0)
	
	I2C0_C1&=~I2C_TX;						//SET AS RX
	I2C_wait();								//WAIT ACK (Check whether to wait ack or send ack 0)
	I2C0_C1|=(1<<3);						//DISABLE ACK TRANSMISSION
	
	data = I2C0_D;							//READ I2C0_D 
	I2C_wait();								//WAIT INTERRUPT
	
	I2C0_C1 &= ~I2C_START;					//SEND STOP BIT
	I2C0_C1 &= ~0x10;						//ENABLE AS TX	
	
	unosnops(200);
	return data;
}

void I2C_wait(void)
{
	while((I2C0_S&(0x2))==0){}
	I2C0_S|=(0x2);
}
