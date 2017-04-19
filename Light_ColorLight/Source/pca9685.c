/*
 * pca9685.c
 *
 *  Created on: Apr 7, 2017
 *      Author: Peter Visser
 */

#include "pca9685.h"
#include "AppHardwareApi.h"
#include "os.h"
#include "dbg.h"


#define TRACE_PCA9685 FALSE

static uint8_t _i2caddr;

PRIVATE uint8 i2c_read(uint8 dev_addr, uint8 *buff, uint8 len);
PRIVATE uint8 i2c_write(uint8 dev_addr, uint8 *data, uint8 len);
PRIVATE void delay_ms (uint16_t ms);

PUBLIC void pca9685_begin(void){

	//	The operating frequency, derived from the 16MHz peripheral clock using the
	//	specified prescaler u8PreScaler, is given by:
	//	Operating frequency = 16mhz/((PreScaler + 1) x 5) MHz
	//	The prescaler is an 8-bit value

	// 31 = 100.000
	// 15 = 200.000

	vAHI_SiMasterConfigure(TRUE, FALSE, 31);
	vAHI_SiSetLocation(TRUE);  //D16,D17 as i2c

	DBG_vPrintf(TRACE_PCA9685, "pca9685_begin \n");

	_i2caddr = 0x40;

	// Reset
	pca9685_reset();
}

PUBLIC void pca9685_reset(void){

	DBG_vPrintf(TRACE_PCA9685, "pca9685_reset \n");

	uint8 s_data[] = {PCA9685_MODE1, 0x0};
	i2c_write(_i2caddr, s_data, 2);
}

PUBLIC void pca9685_setPWMFreq(float freq){

	DBG_vPrintf(TRACE_PCA9685, "pca9685_setPWMFreq \n");
	//Serial.print("Attempting to set freq ");
	//Serial.println(freq);
	freq *= 0.9;  // Correct for overshoot in the frequency setting (see issue #11).
	float prescaleval = 25000000;
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;

	uint8_t prescale = (uint8_t)prescaleval;

	uint8_t oldmode = 0x0;

	uint8 dta_send[] = {PCA9685_MODE1};

	i2c_write(_i2caddr, dta_send, 1);
	i2c_read(_i2caddr, &oldmode, 1);


	uint8_t sleep = (oldmode&0x7F) | 0x10; // sleep

	uint8_t dta_sleep[] = {PCA9685_MODE1, sleep};
	i2c_write(_i2caddr, dta_sleep, 2);

	uint8_t dta_prescale[] = {PCA9685_PRESCALE, prescale};
	i2c_write(_i2caddr, dta_prescale, 2);

	uint8_t dta_oldmode[] = {PCA9685_MODE1, oldmode};
	i2c_write(_i2caddr, dta_oldmode, 2);

	delay_ms(50);

	uint8_t dta_autoIncrement[] = {PCA9685_MODE1, oldmode | 0xa1};
	i2c_write(_i2caddr, dta_autoIncrement, 2);


}


PUBLIC void pca9685_setPWM(uint8_t num, uint16_t on, uint16_t off){

	DBG_vPrintf(TRACE_PCA9685, "pca9685_setPWM \n");

	uint8_t dta_pwm[] =
	{
			LED0_ON_L + (4*num),
			on,
			on>>8,
			off,
			off>>8,
	};


	i2c_write(_i2caddr, dta_pwm, 5);

}



PUBLIC void pca9685_setPin(uint8_t num, uint16_t val, bool invert){

	DBG_vPrintf(TRACE_PCA9685, "pca9685_setPin  \n");


	if (invert) {
		if (val == 0) {
			// Special value for signal fully on.
			pca9685_setPWM(num, 4096, 0);
		}
		else if (val == 4095) {
			// Special value for signal fully off.
			pca9685_setPWM(num, 0, 4095);
		}
		else {
			pca9685_setPWM(num, 0, 4095-val);
		}
	}
	else {
		if (val == 4095) {
			// Special value for signal fully on.
			pca9685_setPWM(num, 4096, 0);
		}
		else if (val == 0) {
			// Special value for signal fully off.
			pca9685_setPWM(num, 0, 4095);
		}
		else {
			pca9685_setPWM(num, 0, val);
		}
	}
}

void calcOnOff(uint16_t *in, bool_t *inv, uint16_t *on, uint16_t *off );
void setPWM_RGB(uint8_t *num, uint16_t *onRed, uint16_t *offRed, uint16_t *onGreen, uint16_t *offGreen, uint16_t *onBlue, uint16_t *offBlue);

PUBLIC void pca9685_setRgb(uint8_t num, uint16_t red, uint16_t green, uint16_t blue, bool_t invert){

	uint16_t onRed, offRed, onGreen, offGreen, onBlue,  offBlue;
	bool_t inv = invert;


	calcOnOff(&red, &inv, &onRed, &offRed);
	calcOnOff(&green, &inv, &onGreen, &offGreen);
	calcOnOff(&blue, &inv, &onBlue, &offBlue);

	setPWM_RGB(&num, &onRed, &offRed,  &onGreen, &offGreen, &onBlue, &offBlue);

}

void calcOnOff(uint16_t *in, bool_t *inv, uint16_t *on, uint16_t *off ){

	// 4096 = special value for fully on/off

	*in = MAX(0, *in);
	*in = MIN(4095, *in);

	if (*inv) {
		if (*in == 0) {

			*on = 4096;
			*off = 0;
		}
		else if (*in == 4095) {

			*on = 0;
			*off = 4095;
		}
		else {

			*on = 0;
			*off = 4095 - *in;
		}
	}
	else {
		if (*in == 4095) {


			*on = 4096;
			*off = 0;
		}
		else if (*in == 0) {

			*on = 0;
			*off = 4095;

		}
		else {

			*on = 0;
			*off = *in;
		}
	}

}

void setPWM_RGB(uint8_t *num, uint16_t *onRed, uint16_t *offRed, uint16_t *onGreen, uint16_t *offGreen, uint16_t *onBlue, uint16_t *offBlue){

	uint8_t dta_pwm[] =
	{
			//
			LED0_ON_L + (4*(*num)),

			*onRed,
			*onRed>>8,
			*offRed,
			*offRed>>8,

			*onGreen,
			*onGreen>>8,
			*offGreen,
			*offGreen>>8,

			*onBlue,
			*onBlue>>8,
			*offBlue,
			*offBlue>>8,




	};


	i2c_write(_i2caddr, dta_pwm, 13);

}





/*
 * write a buff to I2C
 * - i2c_device: i2c device pointer
 * - dev_addr: device address
 * - data: data buff
 * - len: data lenght
 */
PRIVATE uint8 i2c_write(uint8 dev_addr, uint8 *data, uint8 len)
{
	DBG_vPrintf(TRACE_PCA9685, "i2c_write \n");

	vAHI_SiMasterWriteSlaveAddr(dev_addr, FALSE);


	// bSetSTA,  bSetSTO,  bSetRD,  bSetWR,  bSetAckCtrl,  bSetIACK);
	bAHI_SiMasterSetCmdReg(TRUE, FALSE, FALSE, TRUE, E_AHI_SI_SEND_ACK, E_AHI_SI_NO_IRQ_ACK);
	while(bAHI_SiMasterPollTransferInProgress()); //Waitforanindicationofsuccess

	int i;
	uint8 *old = data;
	for(i = 0; i < len; i++)
	{
		vAHI_SiMasterWriteData8(*data++);
		if(i == (len - 1))  //should send stop
		{
			bAHI_SiMasterSetCmdReg(FALSE, TRUE, FALSE, TRUE, E_AHI_SI_SEND_ACK, E_AHI_SI_NO_IRQ_ACK);
		} else
		{
			bAHI_SiMasterSetCmdReg(FALSE, FALSE, FALSE, TRUE, E_AHI_SI_SEND_ACK, E_AHI_SI_NO_IRQ_ACK);
		}
		while(bAHI_SiMasterPollTransferInProgress()); //Waitforanindicationofsuccess
		if(bAHI_SiMasterCheckRxNack())
		{
			bAHI_SiMasterSetCmdReg(FALSE, TRUE, FALSE, FALSE, E_AHI_SI_SEND_ACK, E_AHI_SI_NO_IRQ_ACK);
			break;
		}
	}
	return data - old;
}


/*
 * read a buff to I2C
 * - i2c_device: i2c device pointer
 * - dev_addr: device address
 * - data: data buff
 * - len: data lenght
 * return
 */
PRIVATE uint8 i2c_read(uint8 dev_addr, uint8 *buff, uint8 len)
{
	DBG_vPrintf(TRACE_PCA9685, "i2c_read \n");

	vAHI_SiMasterWriteSlaveAddr(dev_addr, TRUE);
	// bSetSTA,  bSetSTO,  bSetRD,  bSetWR,  bSetAckCtrl,  bSetIACK);
	bAHI_SiMasterSetCmdReg(TRUE, FALSE, FALSE, TRUE, E_AHI_SI_SEND_ACK, E_AHI_SI_NO_IRQ_ACK);
	while(bAHI_SiMasterPollTransferInProgress()); //Waitforanindicationofsuccess

	int i;
	uint8 *old = buff;
	for(i = 0; i < len; i++)
	{
		if(i == (len - 1))  //should send stop, nack
		{
			bAHI_SiMasterSetCmdReg(FALSE, TRUE, TRUE, FALSE, E_AHI_SI_SEND_NACK, E_AHI_SI_NO_IRQ_ACK);
		} else
		{
			bAHI_SiMasterSetCmdReg(FALSE, FALSE, TRUE, FALSE, E_AHI_SI_SEND_ACK, E_AHI_SI_NO_IRQ_ACK);
		}
		while(bAHI_SiMasterPollTransferInProgress()); //Waitforanindicationofsuccess
		*buff++ = u8AHI_SiMasterReadData8();
	}
	return buff - old;
}


void delay_ms (uint16_t ms)
{

	DBG_vPrintf(TRUE, "\n\nStart delay : %d ms\n", ms );

	uint16_t delay;
	volatile uint32_t i;
	for (delay = ms; delay >0 ; delay--)
		//1ms loop with -Os optimisation
	{
		for (i=3500; i >0;i--){};
	}

	DBG_vPrintf(TRUE, "End delay : %d ms\n\n", ms );
}

