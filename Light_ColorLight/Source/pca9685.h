/*
 * i2c.h
 *
 *  Created on: Apr 7, 2017
 *      Author: Peter Visser
 */

#include <jendefs.h>

#ifndef I2C_H_
#define I2C_H_

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9


//PRIVATE uint8_t read8(uint8_t addr);
//PRIVATE void write8(uint8_t addr, uint8_t d);

PUBLIC void pca9685_begin(void);
PUBLIC void pca9685_reset(void);
PUBLIC void pca9685_setPWMFreq(float freq);
PUBLIC void pca9685_setPWM(uint8_t num, uint16_t on, uint16_t off);
PUBLIC void pca9685_setPin(uint8_t num, uint16_t val, bool invert);

PUBLIC void pca9685_setRgb(uint8_t num, uint16_t red, uint16_t green, uint16_t blue, bool_t invert);

#endif /* I2C_H_ */
