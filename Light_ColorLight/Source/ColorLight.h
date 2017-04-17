/*
 * ColorLight.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Peter Visser
 */

#ifndef COLORLIGHT_H_
#define COLORLIGHT_H_


typedef struct
{
	tsZLL_ColourLightDevice light;
	tsIdentifyColour effect;

}rgb_endpoint;



PUBLIC void rgb_setLevels_current(tsZLL_ColourLightDevice light);

PUBLIC void rgb_setLevels(bool_t bOn, uint8 u8Level, uint8 u8Red, uint8 u8Green, uint8 u8Blue);

PUBLIC void rgb_handleIdentify(tsZLL_ColourLightDevice light, tsIdentifyColour *sIdEffect);

PUBLIC void rgb_startEffect(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect, uint8 u8Effect);

PUBLIC void rgb_effectTick(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect);




#endif /* COLORLIGHT_H_ */
