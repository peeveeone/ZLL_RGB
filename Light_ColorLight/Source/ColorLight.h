/*
 * ColorLight.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Peter Visser
 */

#ifndef COLORLIGHT_H_
#define COLORLIGHT_H_

#include "Interpolate.h"

typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

}rgb_state;

typedef struct{

	uint8_t level;
	bool_t isOn;

}light_state;

typedef struct {

	uint8_t deviceAddress;
	uint8_t firstPinAddress;
	bool_t invert;


}light_addressing;

typedef struct
{
	tsZLL_ColourLightDevice light;
	tsIdentifyColour effect;
	tsLI_Vars vars ;
	light_state lightSate;
	rgb_state rgbState;
	light_addressing address;

}rgb_endpoint;



PUBLIC void rgb_setLevels_current(rgb_endpoint* endpoint);

PUBLIC void rgb_setLevels(rgb_endpoint* endpoint, bool_t bOn, uint8 u8Level, uint8 u8Red, uint8 u8Green, uint8 u8Blue);

PUBLIC void rgb_handleIdentify(rgb_endpoint* endpoint);

PUBLIC void rgb_startEffect(rgb_endpoint* endpoint, uint8 u8Effect);

PUBLIC void rgb_effectTick(rgb_endpoint* endpoint);




#endif /* COLORLIGHT_H_ */
