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



/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

PRIVATE tsZLL_ColourLightDevice sLight;
PRIVATE tsIdentifyColour sIdEffect;

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


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
PRIVATE void vOverideProfileId(uint16* pu16Profile, uint8 u8Ep);


/****************************************************************************
 **
 ** NAME: eApp_ZLL_RegisterEndpoint
 **
 ** DESCRIPTION:
 ** Register ZLL endpoints
 **
 ** PARAMETER
 ** Type                                Name                    Descirption
 ** tfpZCL_ZCLCallBackFunction            fptr                    Pointer to ZCL Callback function
 ** tsZLL_CommissionEndpoint            psCommissionEndpoint    Pointer to Commission Endpoint
 **
 **
 ** RETURNS:
 ** teZCL_Status
 *
 ****************************************************************************/
PUBLIC teZCL_Status eApp_ZLL_RegisterEndpoint(tfpZCL_ZCLCallBackFunction fptr, tsZLL_CommissionEndpoint* psCommissionEndpoint)
{
	sDeviceTable.asDeviceRecords[0].u64IEEEAddr = *((uint64*)pvAppApiGetMacAddrLocation());

	ZPS_vAplZdoRegisterProfileCallback(vOverideProfileId);
	zps_vSetIgnoreProfileCheck();

	eZLL_RegisterCommissionEndPoint(LIGHT_COLORLIGHT_COMMISSION_ENDPOINT,fptr,psCommissionEndpoint);

	eZLL_RegisterColourLightEndPoint(LIGHT_COLORLIGHT_LIGHT_00_ENDPOINT,fptr,&sLight);

	/* Initialise the strings in Basic */
	memcpy(sLight.sBasicServerCluster.au8ManufacturerName, "NXP", CLD_BAS_MANUF_NAME_SIZE);
	memcpy(sLight.sBasicServerCluster.au8ModelIdentifier, "ZLL-ColorLight", CLD_BAS_MODEL_ID_SIZE);
	memcpy(sLight.sBasicServerCluster.au8DateCode, "20150212", CLD_BAS_DATE_SIZE);
	memcpy(sLight.sBasicServerCluster.au8SWBuildID, "1000-0003", CLD_BAS_SW_BUILD_SIZE);

	sLight.sLevelControlServerCluster.u8CurrentLevel = 0xFF;
	sLight.sOnOffServerCluster.bOnOff = TRUE;
	sIdEffect.u8Effect = E_CLD_IDENTIFY_EFFECT_STOP_EFFECT;
	sIdEffect.u8Tick = 0;

	return E_ZCL_SUCCESS;
}


/****************************************************************************
 *
 * NAME: vOverideProfileId
 *
 * DESCRIPTION: Allows the application to over ride the profile in the
 * simple descriptor (0xc05e) with the ZHA profile id (0x0104)
 * required for on air packets
 *
 *
 * PARAMETER: pointer to the profile  to be used, the end point sending the data
 *
 * RETURNS: void
 *
 ****************************************************************************/
PRIVATE void vOverideProfileId(uint16* pu16Profile, uint8 u8Ep)
{
	if (u8Ep == LIGHT_COLORLIGHT_LIGHT_00_ENDPOINT)
	{
		*pu16Profile = 0x0104;
	}
}



PUBLIC void APP_ZCL_vSetIdentifyTime(uint16 u16Time)
{
	sLight.sIdentifyServerCluster.u16IdentifyTime = u16Time;
}



PUBLIC bool APP_notIdentifying(){

	return	sLight.sIdentifyServerCluster.u16IdentifyTime == 0;
}


/****************************************************************************
 *
 * NAME: APP_vHandleIdentify
 *
 * DESCRIPTION:
 * ZLL Device Specific identify
 *
 * PARAMETER: the identify time
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void APP_vHandleIdentify() {


	rgb_handleIdentify(&sIdEffect, sLight);

}

/****************************************************************************
 *
 * NAME: vIdEffectTick
 *
 * DESCRIPTION:
 * ZLL Device Specific identify tick
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vIdEffectTick(uint8 u8Endpoint) {

	if (u8Endpoint != LIGHT_COLORLIGHT_LIGHT_00_ENDPOINT) {
		return;
	}

	rgb_effectTick(&sIdEffect, sLight);
}

/****************************************************************************
 *
 * NAME: vStartEffect
 *
 * DESCRIPTION:
 * ZLL Device Specific identify effect set up
 *
 * PARAMETER: void
 *
 * RETURNS: void
 *
 ****************************************************************************/
PUBLIC void vStartEffect(uint8 u8Effect) {

	rgb_startEffect(sLight, &sIdEffect, u8Effect);
}


/****************************************************************************
 *
 * NAME: vRGBLight_SetLevels
 *
 * DESCRIPTION:
 * Set the RGB and levels
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/

PUBLIC void vRGBLight_SetLevels_current(){

	rgb_setLevels_current(sLight);

}




/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
