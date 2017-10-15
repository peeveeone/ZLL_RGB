/*****************************************************************************
 *
 * MODULE:             JN-AN-1171
 *
 * COMPONENT:          App_Light_ColorLight.c
 *
 * DESCRIPTION:        ZLL Demo: Colored Light - Implementation
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5164,
 * JN5161, JN5148, JN5142, JN5139].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2012. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
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
#include "pca9685.h"



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


PRIVATE rgb_endpoint endpoint_01 = {

		.address.deviceAddress = 0x40,
		.address.firstPinAddress = 0,
		.address.invert = INVERT_ENDPOINTS
};

PRIVATE rgb_endpoint endpoint_02 = {

		.address.deviceAddress = 0x40,
		.address.firstPinAddress = 3,
		.address.invert = INVERT_ENDPOINTS
};

PRIVATE rgb_endpoint endpoint_03 = {

		.address.deviceAddress = 0x40,
		.address.firstPinAddress = 6,
		.address.invert = INVERT_ENDPOINTS
};

PRIVATE rgb_endpoint endpoint_04 = {

		.address.deviceAddress = 0x40,
		.address.firstPinAddress = 9,
		.address.invert = INVERT_ENDPOINTS
};

PRIVATE int endpoints[NUMBER_ENDPOINTS] = {


		LIGHT_COLORLIGHT_LIGHT_01_ENDPOINT,
#if NUMBER_ENDPOINTS > 1
		LIGHT_COLORLIGHT_LIGHT_02_ENDPOINT,
#if NUMBER_ENDPOINTS > 2
		LIGHT_COLORLIGHT_LIGHT_03_ENDPOINT,
#if NUMBER_ENDPOINTS > 3
		LIGHT_COLORLIGHT_LIGHT_04_ENDPOINT ,
#endif
#endif
#endif

};

PRIVATE rgb_endpoint *getEndpoint(uint8 epId);
PRIVATE void registerEndpoint(tfpZCL_ZCLCallBackFunction fptr, uint8 epId);
PRIVATE bool isEndpoint(uint8 epId);

PRIVATE bool isEndpoint(uint8 epId){

	bool isEp = (epId >= LIGHT_COLORLIGHT_LIGHT_01_ENDPOINT && epId <= LIGHT_COLORLIGHT_LIGHT_04_ENDPOINT );

	return isEp;
}

PRIVATE rgb_endpoint *getEndpoint(uint8 epId){

	//DBG_vPrintf(TRACE_LIGHT_TASK, "getEndpoint %d \n", epId);

	if(epId == LIGHT_COLORLIGHT_LIGHT_01_ENDPOINT)
		return &endpoint_01;

	if(epId == LIGHT_COLORLIGHT_LIGHT_02_ENDPOINT)
		return &endpoint_02;

	if(epId == LIGHT_COLORLIGHT_LIGHT_03_ENDPOINT)
		return &endpoint_03;

	if(epId == LIGHT_COLORLIGHT_LIGHT_04_ENDPOINT)
		return &endpoint_04;


	//DBG_vPrintf(TRACE_LIGHT_TASK, "getEndpoint : DEFAULT  \n");

	return NULL;
}




tsCLD_ZllDeviceTable sDeviceTable = { ZLL_NUMBER_DEVICES,
		{
				{ 0, ZLL_PROFILE_ID,COLOUR_LIGHT_DEVICE_ID,LIGHT_COLORLIGHT_LIGHT_01_ENDPOINT,2,0,0}
#if NUMBER_ENDPOINTS > 1
				,{ 0, ZLL_PROFILE_ID,COLOUR_LIGHT_DEVICE_ID,LIGHT_COLORLIGHT_LIGHT_02_ENDPOINT,2,0,0}
#if NUMBER_ENDPOINTS > 2
				,{ 0, ZLL_PROFILE_ID,COLOUR_LIGHT_DEVICE_ID,LIGHT_COLORLIGHT_LIGHT_03_ENDPOINT,2,0,0}
#if NUMBER_ENDPOINTS > 3
				,{ 0, ZLL_PROFILE_ID,COLOUR_LIGHT_DEVICE_ID,LIGHT_COLORLIGHT_LIGHT_04_ENDPOINT,2,0,0}
#endif
#endif
#endif
		}

};



PRIVATE void vOverideProfileId(uint16* pu16Profile, uint8 u8Ep);


PUBLIC teZCL_Status eApp_ZLL_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr, tsZLL_CommissionEndpoint* psCommissionEndpoint)
{
	DBG_vPrintf(TRACE_LIGHT_TASK, "eApp_ZLL_RegisterEndpoint  \n");

	pca9685_begin();
	pca9685_setPWMFreq(200);

	sDeviceTable.asDeviceRecords[0].u64IEEEAddr = *((uint64*)pvAppApiGetMacAddrLocation());

	ZPS_vAplZdoRegisterProfileCallback(vOverideProfileId);
	zps_vSetIgnoreProfileCheck();

	eZLL_RegisterCommissionEndPoint(LIGHT_COLORLIGHT_COMMISSION_ENDPOINT,fptr,psCommissionEndpoint);

	int i;


	for (i = 0; i < NUMBER_ENDPOINTS; ++i) {

		registerEndpoint(fptr, endpoints[i]);

	}



	return E_ZCL_SUCCESS;
}

PRIVATE void registerEndpoint(tfpZCL_ZCLCallBackFunction fptr, uint8 epId){

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	DBG_vPrintf(TRACE_LIGHT_TASK, "registerEndpoint : %d \n", epId);


	// set defaults:
	endpoint->vars.sLevel.i32Delta   = 0;
	endpoint->vars.sRed.i32Delta     = 0;
	endpoint->vars.sGreen.i32Delta   = 0;
	endpoint->vars.sBlue.i32Delta    = 0;
	endpoint->vars.sColTemp.i32Delta = 0;
	endpoint->vars.u32PointsAdded  = INTPOINTS;

	endpoint->rgbState.red = 255;
	endpoint->rgbState.green = 225;
	endpoint->rgbState.blue = 255;
	endpoint->lightSate.isOn = TRUE;
	endpoint->lightSate.level = 255;


	ip_setCurrentValues(&endpoint->vars, CLD_LEVELCONTROL_MAX_LEVEL ,253,137,225,4000);


	eZLL_RegisterColourLightEndPoint(epId,fptr, &endpoint->light);

	/* Initialise the strings in Basic */
	memcpy(endpoint->light.sBasicServerCluster.au8ManufacturerName, "PV1", CLD_BAS_MANUF_NAME_SIZE);
	memcpy(endpoint->light.sBasicServerCluster.au8ModelIdentifier, "PeeVeeOne.com", CLD_BAS_MODEL_ID_SIZE);
	memcpy(endpoint->light.sBasicServerCluster.au8DateCode, "20171015", CLD_BAS_DATE_SIZE);
	memcpy(endpoint->light.sBasicServerCluster.au8SWBuildID, "PV1.00001", CLD_BAS_SW_BUILD_SIZE);


	endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
	endpoint->effect.u8Tick = 0;

	endpoint->light.sLevelControlServerCluster.u8CurrentLevel = 0xFF;
	endpoint->light.sOnOffServerCluster.bOnOff = TRUE;



}



PRIVATE void vOverideProfileId(uint16* pu16Profile, uint8 epId)
{
	if (isEndpoint(epId))
	{
		*pu16Profile = 0x0104;
	}
}



PUBLIC void APP_ZCL_vSetIdentifyTime(uint8 epId, uint16 u16Time)
{

	DBG_vPrintf(TRACE_LIGHT_TASK, "APP_ZCL_vSetIdentifyTime  \n");
	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	endpoint->light.sIdentifyServerCluster.u16IdentifyTime = u16Time;
}



PUBLIC bool APP_notIdentifying(uint8 epId){

	DBG_vPrintf(TRACE_LIGHT_TASK, "APP_notIdentifying  \n");

	if(!isEndpoint(epId))
		return FALSE;

	rgb_endpoint* endpoint = getEndpoint(epId);

	return	endpoint->light.sIdentifyServerCluster.u16IdentifyTime == 0;
}



PUBLIC void APP_vHandleIdentify(uint8 epId) {

	DBG_vPrintf(TRACE_LIGHT_TASK, "APP_vHandleIdentify  \n");

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_handleIdentify( endpoint);

}


PUBLIC void vIdEffectTick(uint8 epId) {

	//DBG_vPrintf(TRACE_LIGHT_TASK, "vIdEffectTick  \n");

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_effectTick(endpoint);
}


PUBLIC void vStartEffect(uint8 epId, uint8 u8Effect) {

	DBG_vPrintf(TRACE_LIGHT_TASK, "vStartEffect  \n");

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_startEffect(endpoint, u8Effect);
}



PUBLIC void vRGBLight_SetLevels_current(uint8 epId){

	DBG_vPrintf(TRACE_LIGHT_TASK, "vRGBLight_SetLevels_current  \n");

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_setLevels_current(endpoint);

}

void rgb_setLevel(rgb_endpoint *endpoint, uint32 level, uint32 u32Red, uint32 u32Green, uint32 u32Blue);

PUBLIC void vCreateInterpolationPoints( void){


	int i;

	for (i = 0; i < NUMBER_ENDPOINTS; ++i) {

		if(isEndpoint(endpoints[i]))
		{

			//DBG_vPrintf(TRACE_LIGHT_TASK, "vCreateInterpolationPoints - ep :%d\n", i);

			rgb_endpoint* endpoint = getEndpoint(endpoints[i]);

			ip_createPoints(&endpoint->vars);

			rgb_setLevel(endpoint,
					endpoint->vars.sLevel.u32Current >> SCALE,
					endpoint->vars.sRed.u32Current >> SCALE,
					endpoint->vars.sGreen.u32Current >> SCALE,
					endpoint->vars.sBlue.u32Current >> SCALE);
		} else{

			DBG_vPrintf(TRACE_LIGHT_TASK, "vCreateInterpolationPoints - ep :%d NOT AN ENDPOINT\n", i);
		}

	}
}

void rgb_setLevel(rgb_endpoint *endpoint, uint32 level, uint32 u32Red, uint32 u32Green, uint32 u32Blue)
{
	//DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_setLevel  \n");

	bool_t needsUpdate = FALSE;

	needsUpdate = endpoint->rgbState.red != (uint8) u32Red ||
			endpoint->rgbState.green !=(uint8) u32Green ||
			endpoint->rgbState.blue != (uint8) u32Blue||
			endpoint->lightSate.level != (uint8) level ||
			endpoint->light.sOnOffServerCluster.bOnOff != endpoint->lightSate.isOn;

	if(needsUpdate){

		uint8   u8Red, u8Green,u8Blue;
		uint16_t red, green, blue = 0;

		/* Note the new values */
		endpoint->rgbState.red   = (uint8) u32Red;
		endpoint->rgbState.green = (uint8) u32Green;
		endpoint->rgbState.blue  = (uint8) u32Blue;
		endpoint->lightSate.level = (uint8) MAX(1, level);
		//endpoint->lightSate.isOn = endpoint->light.sOnOffServerCluster.bOnOff;

		if (endpoint->light.sIdentifyServerCluster.u16IdentifyTime == 0)
			endpoint->lightSate.isOn = endpoint->light.sOnOffServerCluster.bOnOff;


		/* Is the lamp on ? */
		if (endpoint->lightSate.isOn)
		{

			/* Set outputs */

			/* Scale colour for brightness level */
			u8Red   = (uint8)(((uint32)endpoint->rgbState.red   * (uint32)endpoint->lightSate.level) / (uint32)255);
			u8Green = (uint8)(((uint32)endpoint->rgbState.green * (uint32)endpoint->lightSate.level) / (uint32)255);
			u8Blue  = (uint8)(((uint32)endpoint->rgbState.blue  * (uint32)endpoint->lightSate.level) / (uint32)255);

			/* Don't allow fully off */
			if (u8Red   == 0) u8Red   = 1;
			if (u8Green == 0) u8Green = 1;
			if (u8Blue  == 0) u8Blue  = 1;



		}
		else /* Turn off */
		{
			u8Red   = 0;
			u8Green = 0;
			u8Blue  = 0;
		}



		// scale to 12 bit
		red =  u8Red << 4 | u8Red >> 4;
		green = u8Green << 4 | u8Green >> 4;
		blue =  u8Blue << 4 | u8Blue >> 4;

		DBG_vPrintf(TRACE_LIGHT_TASK, "rgb_setLevel to output fp: %d isOn:%d r:%d g:%d b:%d \n",endpoint->address.firstPinAddress, endpoint->lightSate.isOn,  u8Red, u8Green, u8Blue);

		pca9685_setRgb(endpoint->address.firstPinAddress, red, green, blue, endpoint->address.invert);



	}

}










/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
