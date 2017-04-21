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

#include "ColorLight.h"
#include "Interpolate.h"


#ifdef DEBUG_LIGHT_TASK
#define TRACE_LIGHT_TASK  TRUE
#else
#define TRACE_LIGHT_TASK FALSE
#endif

#ifdef DEBUG_PATH
#define TRACE_PATH  TRUE
#else
#define TRACE_PATH  FALSE
#endif


PUBLIC void rgb_setLevels_current(rgb_endpoint* endpoint){

	uint8 u8Red, u8Green, u8Blue;

	eCLD_ColourControl_GetRGB(endpoint->light.sEndPoint.u8EndPointNumber,&u8Red, &u8Green, &u8Blue);

	rgb_setLevels(endpoint, endpoint->light.sOnOffServerCluster.bOnOff, endpoint->light.sLevelControlServerCluster.u8CurrentLevel,u8Red,u8Green,u8Blue);

}


PUBLIC void rgb_setLevels(rgb_endpoint* endpoint, bool_t bOn, uint8 u8Level, uint8 u8Red, uint8 u8Green, uint8 u8Blue)
{


	if (bOn == TRUE)
	{
		ip_start(&endpoint->vars,u8Level, u8Red, u8Green, u8Blue, 0 );

	}
	else
	{
		ip_stop(&endpoint->vars);

	}

	endpoint->lightSate.isOn = bOn;

	//vBULB_SetOnOff(bOn);
}


PUBLIC void rgb_handleIdentify(rgb_endpoint* endpoint) {

	uint8 u8Red, u8Green, u8Blue;


	if (endpoint->effect.u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT) {
		/* do nothing */
	}
	else if (endpoint->light.sIdentifyServerCluster.u16IdentifyTime == 0)
	{
		eCLD_ColourControl_GetRGB(endpoint->light.sEndPoint.u8EndPointNumber,&u8Red, &u8Green, &u8Blue);

		rgb_setLevels(endpoint, endpoint->light.sOnOffServerCluster.bOnOff,endpoint->light.sLevelControlServerCluster.u8CurrentLevel,u8Red,u8Green,u8Blue);
	}
	else
	{
		/* Set the Identify levels */

		rgb_setLevels(endpoint, TRUE, 159, 250, 0, 0);
	}
}


PUBLIC void rgb_startEffect(rgb_endpoint* endpoint, uint8 u8Effect) {


	switch (u8Effect) {
	case E_CLD_IDENTIFY_EFFECT_BLINK:

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_BLINK \n");

		endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_BLINK;
		endpoint->effect.u8Level = 250;
		endpoint->effect.u8Red = 255;
		endpoint->effect.u8Green = 0;
		endpoint->effect.u8Blue = 0;
		endpoint->effect.bFinish = FALSE;


		endpoint->light.sIdentifyServerCluster.u16IdentifyTime = 2;

		endpoint->effect.u8Tick = 10;
		break;
	case E_CLD_IDENTIFY_EFFECT_BREATHE:

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_BREATHE \n");

		endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_BREATHE;
		endpoint->effect.bDirection = 1;
		endpoint->effect.bFinish = FALSE;
		endpoint->effect.u8Level = 0;
		endpoint->effect.u8Count = 15;
		eCLD_ColourControl_GetRGB( endpoint->light.sEndPoint.u8EndPointNumber, &endpoint->effect.u8Red, &endpoint->effect.u8Green, &endpoint->effect.u8Blue);

		endpoint->light.sIdentifyServerCluster.u16IdentifyTime = 17;

		endpoint->effect.u8Tick = 200;
		break;
	case E_CLD_IDENTIFY_EFFECT_OKAY:

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_OKAY \n");

		endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_OKAY;
		endpoint->effect.bFinish = FALSE;
		endpoint->effect.u8Level = 250;
		endpoint->effect.u8Red = 0;
		endpoint->effect.u8Green = 255;
		endpoint->effect.u8Blue = 0;

		endpoint->light.sIdentifyServerCluster.u16IdentifyTime = 2;

		endpoint->effect.u8Tick = 10;
		break;
	case E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE:

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_BLINK \n");

		endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_CHANNEL_CHANGE;
		endpoint->effect.u8Level = 250;
		endpoint->effect.u8Red = 255;
		endpoint->effect.u8Green = 127;
		endpoint->effect.u8Blue = 4;
		endpoint->effect.bFinish = FALSE;

		endpoint->light.sIdentifyServerCluster.u16IdentifyTime = 9;
		endpoint->effect.u8Tick = 80;
		break;

	case E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT:

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_FINISH_EFFECT \n");

		if (endpoint->effect.u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT)
		{

			DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_STOP_EFFECT \n");

			endpoint->effect.bFinish = TRUE;
		}
		break;
	case E_CLD_IDENTIFY_EFFECT_STOP_EFFECT:

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_startEffect : E_CLD_IDENTIFY_EFFECT_STOP_EFFECT \n");

		endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
		endpoint->light.sIdentifyServerCluster.u16IdentifyTime = 1;
		break;
	}
}

PUBLIC void rgb_effectTick(rgb_endpoint* endpoint) {

	if (endpoint->effect.u8Effect < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT)
	{
		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_effectTick : 1 < E_CLD_IDENTIFY_EFFECT_STOP_EFFECT \n");

		if (endpoint->effect.u8Tick > 0)
		{

			endpoint->effect.u8Tick--;
			/* Set the light parameters */
			rgb_setLevels(endpoint, TRUE, endpoint->effect.u8Level,endpoint->effect.u8Red,endpoint->effect.u8Green,endpoint->effect.u8Blue);
			/* Now adjust parameters ready for for next round */
			switch (endpoint->effect.u8Effect) {
			case E_CLD_IDENTIFY_EFFECT_BLINK:


				break;

			case E_CLD_IDENTIFY_EFFECT_BREATHE:
				DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_effectTick : E_CLD_IDENTIFY_EFFECT_BREATHE \n");

				if (endpoint->effect.bDirection) {
					if (endpoint->effect.u8Level >= 250) {
						endpoint->effect.u8Level -= 50;
						endpoint->effect.bDirection = 0;
					} else {
						endpoint->effect.u8Level += 50;
					}
				} else {
					if (endpoint->effect.u8Level == 0) {
						// go back up, check for stop
						endpoint->effect.u8Count--;
						if ((endpoint->effect.u8Count) && ( !endpoint->effect.bFinish)) {
							endpoint->effect.u8Level += 50;
							endpoint->effect.bDirection = 1;
						} else {
							//DBG_vPrintf(TRACE_LIGHT_TASK, "\n>>set tick 0<<");
							/* lpsw2773 - stop the effect on the next tick */
							endpoint->effect.u8Tick = 0;
						}
					} else {
						endpoint->effect.u8Level -= 50;
					}
				}
				break;
			default:
				if ( endpoint->effect.bFinish ) {
					endpoint->effect.u8Tick = 0;
				}
			}
		} else {

			DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_effectTick : E_CLD_IDENTIFY_EFFECT_STOP_EFFECT \n");

			endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
			endpoint->effect.bDirection = FALSE;
			endpoint->light.sIdentifyServerCluster.u16IdentifyTime = 0;

			uint8 u8Red, u8Green, u8Blue;

			eCLD_ColourControl_GetRGB(endpoint->light.sEndPoint.u8EndPointNumber,&u8Red, &u8Green, &u8Blue);

			rgb_setLevels(endpoint, endpoint->light.sOnOffServerCluster.bOnOff,
					endpoint->light.sLevelControlServerCluster.u8CurrentLevel,
					u8Red,
					u8Green,
					u8Blue);
		}
	}
}



