/*
/******************************************************************************
* NXP Semiconductors
* (c) Copyright 2016 NXP Semiconductors
* ALL RIGHTS RESERVED.
*******************************************************************************
Services performed by NXP in this matter are performed AS IS and without
any warranty. CUSTOMER retains the final decision relative to the total design
and functionality of the end product. NXP neither guarantees nor will be
held liable by CUSTOMER for the success of this project.
NXP DISCLAIMS ALL WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY INCLUDING,
BUT NOT LIMITED TO, IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR
A PARTICULAR PURPOSE ON ANY HARDWARE, SOFTWARE ORE ADVISE SUPPLIED TO THE
PROJECT BY NXP, AND OR NAY PRODUCT RESULTING FROM NXP SERVICES.
IN NO EVENT SHALL NXP BE LIABLE FOR INCIDENTAL OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THIS AGREEMENT.
CUSTOMER agrees to hold NXP harmless against any and all claims demands
or actions by anyone on account of any damage, or injury, whether commercial,
contractual, or tortuous, rising directly or indirectly as a result of the
advise or assistance supplied CUSTOMER in connection with product, services
or goods supplied under this Agreement.
*******************************************************************************
* File: main.c
* Owner: David Diaz
* Version: 1.0
* Date: May-2016
* Classification General Business Information
* FXOS8700CQ example code with use of FRDM-K64F
*******************************************************************************
* This code has been written for the NXP FRDM-K64F
* boards and demonstrates how to read the magnetic data from the FXOS8700CQ
* acceleration by I2C using an interrupt technique.
*
*
* The calculated values can be also watched in the FreeMASTER application.
*******************************************************************************
Revision History:
Version                Date                                     Author
1.0                    May-2016  	           				    David Diaz
******************************************************************************/

#include "C:\Users\jonac\Desktop\FRDM-K64F_FXOS8700CQ_BasicReadUsingInterrupts\FRDM-K64F_FXOS8700CQ_BasicReadUsingInterrupts\Includes\MK64F12.h"
#include "C:\Users\jonac\Desktop\FRDM-K64F_FXOS8700CQ_BasicReadUsingInterrupts\FRDM-K64F_FXOS8700CQ_BasicReadUsingInterrupts\Includes\I2C.h"
#include "C:\Users\jonac\Desktop\FRDM-K64F_FXOS8700CQ_BasicReadUsingInterrupts\FRDM-K64F_FXOS8700CQ_BasicReadUsingInterrupts\Includes\FXOS8700CQ.h"

/******************************************************************************
* Global variables and constants
******************************************************************************/

unsigned char AccelData[6];

short Xout_Accel_14_bit, Yout_Accel_14_bit, Zout_Accel_14_bit;
float Xout_g, Yout_g, Zout_g;

char DataReady, WhoAmI;

/******************************************************************************
* Functions
******************************************************************************/

void MCU_Init(void);
void FXOS8700CQ_Init (void);
void FXOS8700CQ_Accel_Calibration (void);

/******************************************************************************
* Main
******************************************************************************/
int main(void)
{
	MCU_Init();
	FXOS8700CQ_Init();
	FXOS8700CQ_Accel_Calibration();
	WhoAmI = I2C_ReadRegister(FXOS8700CQ_I2C_ADDRESS, WHO_AM_I_REG);

	while(1)
	    {
			if (DataReady)		// Is a new set of data ready?
			{
				DataReady = 0;

				I2C_ReadMultiRegisters(FXOS8700CQ_I2C_ADDRESS, OUT_X_MSB_REG, 12, AccelData);		// Read data output registers 0x01-0x06 and 0x33 - 0x38

				// 14-bit accelerometer data
				Xout_Accel_14_bit = ((short) (AccelData[0]<<8 | AccelData[1])) >> 2;		// Compute 14-bit X-axis acceleration output value
				Yout_Accel_14_bit = ((short) (AccelData[2]<<8 | AccelData[3])) >> 2;		// Compute 14-bit Y-axis acceleration output value
				Zout_Accel_14_bit = ((short) (AccelData[4]<<8 | AccelData[5])) >> 2;		// Compute 14-bit Z-axis acceleration output value

				// Accelerometer data converted to g's
				Xout_g = ((float) Xout_Accel_14_bit) / SENSITIVITY_2G;		// Compute X-axis output value in g's
				Yout_g = ((float) Yout_Accel_14_bit) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
				Zout_g = ((float) Zout_Accel_14_bit) / SENSITIVITY_2G;		// Compute Z-axis output value in g's

			}
		}
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////


/******************************************************************************
* MCU initialization function
******************************************************************************/
void MCU_Init(void){
	//I2C1 module initialization
	SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;		// Turn on clock to I2C1 module
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;		// Turn on clock to Port C module
	PORTE_PCR24 |= PORT_PCR_MUX(0x5);		// PTE24 pin is I2C1 SCL line
	PORTE_PCR25 |= PORT_PCR_MUX(0x5);		// PTE25 pin is I2C1 SDA line
	I2C0_F  |= I2C_F_ICR(0x14); 			// SDA hold time = 2.125us, SCL start hold time = 4.25us, SCL stop hold time = 5.125us
	I2C0_C1 |= I2C_C1_IICEN_MASK;    		// Enable I2C1 module

	//Configure the PTC6 pin (connected to the INT1 of the FXOS8700CQ) for falling edge interrupts
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;		// Turn on clock to Port A module
	PORTC_PCR6 |= (0|PORT_PCR_ISF_MASK|		// Clear the interrupt flag
					PORT_PCR_MUX(0x1)|		// PTC6 is configured as GPIO
					PORT_PCR_IRQC(0xA));	// PTC6 is configured for falling edge interrupts

	//Enable PORTD interrupt on NVIC
	NVIC_EnableIRQ(PORTC_IRQn);      		// Enable interrupts
	NVIC_ClearPendingIRQ(PORTC_IRQn);		// Clear pending interrupts
}

/******************************************************************************
* FXOS8700CQ initialization function
******************************************************************************/
void FXOS8700CQ_Init (void)
{
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG2, 0x40);		// Reset all registers to POR values

	Pause(0x631);		// ~1ms delay

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, XYZ_DATA_CFG_REG, 0x00);		// +/-2g range with 0.244mg/LSB

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, M_CTRL_REG1, 0x00);		// Only accelerometer sensor is active

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG2, 0x02);     // High Resolution mode
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG3, 0x00);		// Push-pull, active low interrupt
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG4, 0x01);		// Enable DRDY interrupt
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG5, 0x01);		// DRDY interrupt routed to INT1 - PTD4
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x35);		// ODR = 3.125Hz, Reduced noise, Active mode
}

/******************************************************************************
* Simple accelerometer offset calibration
******************************************************************************/
void FXOS8700CQ_Accel_Calibration (void)
{
	char X_Accel_offset, Y_Accel_offset, Z_Accel_offset;

	DataReady = 0;
	while (!DataReady){}		// Is a first set of data ready?
	DataReady = 0;

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x00);		// Standby mode

	I2C_ReadMultiRegisters(FXOS8700CQ_I2C_ADDRESS, OUT_X_MSB_REG, 6, AccelData);		// Read data output registers 0x01-0x06

	Xout_Accel_14_bit = ((short) (AccelData[0]<<8 | AccelData[1])) >> 2;		// Compute 14-bit X-axis acceleration output value
	Yout_Accel_14_bit = ((short) (AccelData[2]<<8 | AccelData[3])) >> 2;		// Compute 14-bit Y-axis acceleration output value
	Zout_Accel_14_bit = ((short) (AccelData[4]<<8 | AccelData[5])) >> 2;		// Compute 14-bit Z-axis acceleration output value

	X_Accel_offset = Xout_Accel_14_bit / 8 * (-1);		// Compute X-axis offset correction value
	Y_Accel_offset = Yout_Accel_14_bit / 8 * (-1);		// Compute Y-axis offset correction value
	Z_Accel_offset = (Zout_Accel_14_bit - SENSITIVITY_2G) / 8 * (-1);		// Compute Z-axis offset correction value

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, OFF_X_REG, X_Accel_offset);
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, OFF_Y_REG, Y_Accel_offset);
	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, OFF_Z_REG, Z_Accel_offset);

	I2C_WriteRegister(FXOS8700CQ_I2C_ADDRESS, CTRL_REG1, 0x35);		// Active mode again
}

/******************************************************************************
* PORT C Interrupt handler
******************************************************************************/
void PORTC_IRQHandler()
{
	PORTC_PCR6 |= PORT_PCR_ISF_MASK;			// Clear the interrupt flag
	DataReady = 1;
}
