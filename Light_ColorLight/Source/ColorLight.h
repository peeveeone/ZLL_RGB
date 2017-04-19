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

typedef struct
{
	tsZLL_ColourLightDevice light;
	tsIdentifyColour effect;
	tsLI_Vars vars ;
	light_state lightSate;
	rgb_state rgbState;

}rgb_endpoint;





PUBLIC void rgb_setLevels_current(tsZLL_ColourLightDevice light, tsLI_Vars* vars);

PUBLIC void rgb_setLevels(tsLI_Vars* vars, bool_t bOn, uint8 u8Level, uint8 u8Red, uint8 u8Green, uint8 u8Blue);

PUBLIC void rgb_handleIdentify(tsZLL_ColourLightDevice light, tsIdentifyColour *sIdEffect, tsLI_Vars* vars);

PUBLIC void rgb_startEffect(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect, uint8 u8Effect);

PUBLIC void rgb_effectTick(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect, tsLI_Vars* vars);




#endif /* COLORLIGHT_H_ */
