/*
 * ColorLight.c
 *
 *  Created on: Apr 17, 2017
 *      Author: Peter Visser
 */

#include <jendefs.h>
#include "AppHardwareApi.h"
#include "zps_gen.h"
#include "App_Light_ColorLight.h"
#include "app_common.h"
#include "app_zcl_light_task.h"
#include "dbg.h"
#include <string.h>
#include "DriverBulb_Shim.h"
#include "ColorLight.h"
#include "Interpolate.h"

PUBLIC void rgb_setLevels_current(tsZLL_ColourLightDevice light, tsLI_Vars* vars){

	uint8 u8Red, u8Green, u8Blue;

	eCLD_ColourControl_GetRGB(light.sEndPoint.u8EndPointNumber,&u8Red, &u8Green, &u8Blue);

	rgb_setLevels(vars, light.sOnOffServerCluster.bOnOff, light.sLevelControlServerCluster.u8CurrentLevel,u8Red,u8Green,u8Blue);
}

PUBLIC void rgb_setLevels(tsLI_Vars* vars, bool_t bOn, uint8 u8Level, uint8 u8Red, uint8 u8Green, uint8 u8Blue)
{
	if (bOn == TRUE)
	{
		ip_start(vars,u8Level, u8Red, u8Green, u8Blue, 0 );

	}
	else
	{
		ip_stop(vars);

	}

	vBULB_SetOnOff(bOn);
}


PUBLIC void rgb_handleIdentify(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect, tsLI_Vars* vars) {

	uint8 u8Red, u8Green, u8Blue;


	if (sIdEffect->u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT) {
		/* do nothing */
	}
	else if (sLight.sIdentifyServerCluster.u16IdentifyTime == 0)
	{
		eCLD_ColourControl_GetRGB(sLight.sEndPoint.u8EndPointNumber,&u8Red, &u8Green, &u8Blue);

		rgb_setLevels(vars, sLight.sOnOffServerCluster.bOnOff,sLight.sLevelControlServerCluster.u8CurrentLevel,u8Red,u8Green,u8Blue);
	}
	else
	{
		/* Set the Identify levels */

		rgb_setLevels(vars, TRUE, 159, 250, 0, 0);
	}
}


PUBLIC void rgb_startEffect(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect, uint8 u8Effect) {
	switch (u8Effect) {
	case E_CLD_IDENTIFY_EFFECT_BLINK:
		sIdEffect->u8Effect = E_CLD_IDENTIFY_EFFECT_BLINK;
		sIdEffect->u8Level = 250;
		sIdEffect->u8Red = 255;
		sIdEffect->u8Green = 0;
		sIdEffect->u8Blue = 0;
		sIdEffect->bFinish = FALSE;

		sLight.sIdentifyServerCluster.u16IdentifyTime = 2;

		sIdEffect->u8Tick = 10;
		break;
	case E_CLD_IDENTIFY_EFFECT_BREATHE:
		sIdEffect->u8Effect = E_CLD_IDENTIFY_EFFECT_BREATHE;
		sIdEffect->bDirection = 1;
		sIdEffect->bFinish = FALSE;
		sIdEffect->u8Level = 0;
		sIdEffect->u8Count = 15;
		eCLD_ColourControl_GetRGB( sLight.sEndPoint.u8EndPointNumber, &sIdEffect->u8Red, &sIdEffect->u8Green, &sIdEffect->u8Blue);

		sLight.sIdentifyServerCluster.u16IdentifyTime = 17;

		sIdEffect->u8Tick = 200;
		break;
	case E_CLD_IDENTIFY_EFFECT_OKAY:
		sIdEffect->u8Effect = E_CLD_IDENTIFY_EFFECT_OKAY;
		sIdEffect->bFinish = FALSE;
		sIdEffect->u8Level = 250;
		sIdEffect->u8Red = 0;
		sIdEffect->u8Green = 255;
		sIdEffect->u8Blue = 0;

		sLight.sIdentifyServerCluster.u16IdentifyTime = 2;

		sIdEffect->u8Tick = 10;
		break;
	case E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE:
		sIdEffect->u8Effect = E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE;
		sIdEffect->u8Level = 250;
		sIdEffect->u8Red = 255;
		sIdEffect->u8Green = 127;
		sIdEffect->u8Blue = 4;
		sIdEffect->bFinish = FALSE;

		sLight.sIdentifyServerCluster.u16IdentifyTime = 9;
		sIdEffect->u8Tick = 80;
		break;

	case E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT:
		if (sIdEffect->u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT)
		{
			sIdEffect->bFinish = TRUE;
		}
		break;
	case E_CLD_IDENTIFY_EFFECT_STOP_EFFECT:
		sIdEffect->u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
		sLight.sIdentifyServerCluster.u16IdentifyTime = 1;
		break;
	}
}

PUBLIC void rgb_effectTick(tsZLL_ColourLightDevice sLight, tsIdentifyColour *sIdEffect, tsLI_Vars* vars) {


	if (sIdEffect->u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT)
	{
		if (sIdEffect->u8Tick > 0)
		{

			sIdEffect->u8Tick--;
			/* Set the light parameters */
			rgb_setLevels(vars, TRUE, sIdEffect->u8Level,sIdEffect->u8Red,sIdEffect->u8Green,sIdEffect->u8Blue);
			/* Now adjust parameters ready for for next round */
			switch (sIdEffect->u8Effect) {
			case E_CLD_IDENTIFY_EFFECT_BLINK:
				break;

			case E_CLD_IDENTIFY_EFFECT_BREATHE:
				if (sIdEffect->bDirection) {
					if (sIdEffect->u8Level >= 250) {
						sIdEffect->u8Level -= 50;
						sIdEffect->bDirection = 0;
					} else {
						sIdEffect->u8Level += 50;
					}
				} else {
					if (sIdEffect->u8Level == 0) {
						// go back up, check for stop
						sIdEffect->u8Count--;
						if ((sIdEffect->u8Count) && ( !sIdEffect->bFinish)) {
							sIdEffect->u8Level += 50;
							sIdEffect->bDirection = 1;
						} else {
							//DBG_vPrintf(TRACE_LIGHT_TASK, "\n>>set tick 0<<");
							/* lpsw2773 - stop the effect on the next tick */
							sIdEffect->u8Tick = 0;
						}
					} else {
						sIdEffect->u8Level -= 50;
					}
				}
				break;
			default:
				if ( sIdEffect->bFinish ) {
					sIdEffect->u8Tick = 0;
				}
			}
		} else {

			sIdEffect->u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
			sIdEffect->bDirection = FALSE;
			sLight.sIdentifyServerCluster.u16IdentifyTime = 0;

			uint8 u8Red, u8Green, u8Blue;

			eCLD_ColourControl_GetRGB(sLight.sEndPoint.u8EndPointNumber,&u8Red, &u8Green, &u8Blue);

			rgb_setLevels(vars, sLight.sOnOffServerCluster.bOnOff,
					sLight.sLevelControlServerCluster.u8CurrentLevel,
					u8Red,
					u8Green,
					u8Blue);
		}
	}
}



