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

#include "app_light_interpolation.h"
#include "DriverBulb_Shim.h"
#include "ColorLight.h"




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


PRIVATE rgb_endpoint endpoint_01;

PRIVATE rgb_endpoint *getEndpoint(uint8 epId);
PRIVATE void registerEndpoint(tfpZCL_ZCLCallBackFunction fptr, uint8 epId);
PRIVATE bool isEndpoint(uint8 epId);

PRIVATE bool isEndpoint(uint8 epId){

	return (epId == LIGHT_COLORLIGHT_LIGHT_00_ENDPOINT);

}

PRIVATE rgb_endpoint *getEndpoint(uint8 epId){

	return &endpoint_01;
}




tsCLD_ZllDeviceTable sDeviceTable = { ZLL_NUMBER_DEVICES,
		{
				{ 0,
						ZLL_PROFILE_ID,
						COLOUR_LIGHT_DEVICE_ID,
						LIGHT_COLORLIGHT_LIGHT_00_ENDPOINT,
						2,
						0,
						0}
		}
};



PRIVATE void vOverideProfileId(uint16* pu16Profile, uint8 u8Ep);


PUBLIC teZCL_Status eApp_ZLL_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr, tsZLL_CommissionEndpoint* psCommissionEndpoint)
{
	sDeviceTable.asDeviceRecords[0].u64IEEEAddr = *((uint64*)pvAppApiGetMacAddrLocation());

	ZPS_vAplZdoRegisterProfileCallback(vOverideProfileId);
	zps_vSetIgnoreProfileCheck();

	eZLL_RegisterCommissionEndPoint(LIGHT_COLORLIGHT_COMMISSION_ENDPOINT,fptr,psCommissionEndpoint);


	registerEndpoint(fptr, LIGHT_COLORLIGHT_LIGHT_00_ENDPOINT);


	return E_ZCL_SUCCESS;
}

PRIVATE void registerEndpoint(tfpZCL_ZCLCallBackFunction fptr, uint8 epId){

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	eZLL_RegisterColourLightEndPoint(epId,fptr, &endpoint->light);

	/* Initialise the strings in Basic */
	memcpy(endpoint->light.sBasicServerCluster.au8ManufacturerName, "NXP", CLD_BAS_MANUF_NAME_SIZE);
	memcpy(endpoint->light.sBasicServerCluster.au8ModelIdentifier, "ZLL-ColorLight", CLD_BAS_MODEL_ID_SIZE);
	memcpy(endpoint->light.sBasicServerCluster.au8DateCode, "20150212", CLD_BAS_DATE_SIZE);
	memcpy(endpoint->light.sBasicServerCluster.au8SWBuildID, "PV1_RGB_0000", CLD_BAS_SW_BUILD_SIZE);

	endpoint->light.sLevelControlServerCluster.u8CurrentLevel = 0xFF;
	endpoint->light.sOnOffServerCluster.bOnOff = TRUE;
	endpoint->effect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
	endpoint->effect.u8Tick = 0;

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
	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	endpoint->light.sIdentifyServerCluster.u16IdentifyTime = u16Time;
}



PUBLIC bool APP_notIdentifying(uint8 epId){

	if(!isEndpoint(epId))
		return FALSE;

	rgb_endpoint* endpoint = getEndpoint(epId);

	return	endpoint->light.sIdentifyServerCluster.u16IdentifyTime == 0;
}



PUBLIC void APP_vHandleIdentify(uint8 epId) {

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_handleIdentify( endpoint->light, &endpoint->effect);

}


PUBLIC void vIdEffectTick(uint8 epId) {

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_effectTick(endpoint->light, &endpoint->effect);
}


PUBLIC void vStartEffect(uint8 epId, uint8 u8Effect) {

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_startEffect(endpoint->light, &endpoint->effect, u8Effect);
}



PUBLIC void vRGBLight_SetLevels_current(uint8 epId){

	if(!isEndpoint(epId))
		return;

	rgb_endpoint* endpoint = getEndpoint(epId);

	rgb_setLevels_current(endpoint->light);

}




/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
