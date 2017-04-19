/*
 * Interpolate.c
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
#include "Interpolate.h"





void ip_setCurrentValues(tsLI_Vars* sLI_Vars, uint32 u32Level, uint32 u32Red, uint32 u32Green, uint32 u32Blue, uint32 u32ColTemp);
void ip_start(tsLI_Vars* sLI_Vars, uint32 u32Level, uint32 u32Red, uint32 u32Green, uint32 u32Blue, uint32 u32ColTemp);
void ip_stop(tsLI_Vars* sLI_Vars);
void ip_updateDriver(tsLI_Vars* sLI_Vars);
void ip_initVar(tsLI_Params *psLI_Params, uint32 u32NewTarget);
PRIVATE uint32  u32divu10(uint32 n);


PUBLIC void ip_setCurrentValues(tsLI_Vars* sLI_Vars, uint32 u32Level, uint32 u32Red, uint32 u32Green, uint32 u32Blue, uint32 u32ColTemp)
{
	sLI_Vars->sLevel.u32Current   += u32Level   << SCALE;
	sLI_Vars->sRed.u32Current     += u32Red     << SCALE;
	sLI_Vars->sGreen.u32Current   += u32Blue    << SCALE;
    sLI_Vars->sBlue.u32Current    += u32Green   << SCALE;
    sLI_Vars->sColTemp.u32Current += u32ColTemp << SCALE;

}


PUBLIC void ip_start(tsLI_Vars* sLI_Vars, uint32 u32Level, uint32 u32Red, uint32 u32Green, uint32 u32Blue, uint32 u32ColTemp)
{
	ip_initVar(&sLI_Vars->sLevel,    u32Level);
	ip_initVar(&sLI_Vars->sRed,      u32Red);
	ip_initVar(&sLI_Vars->sGreen,    u32Green);
	ip_initVar(&sLI_Vars->sBlue,     u32Blue);
	ip_initVar(&sLI_Vars->sColTemp,  u32ColTemp);
    ip_updateDriver(sLI_Vars);
    sLI_Vars->u32PointsAdded  = 1;
}

PUBLIC void ip_stop(tsLI_Vars* sLI_Vars)
{
    sLI_Vars->u32PointsAdded = INTPOINTS;
}


PUBLIC void ip_createPoints(tsLI_Vars* sLI_Vars)
{
    if (sLI_Vars->u32PointsAdded < INTPOINTS)
    {
    	sLI_Vars->u32PointsAdded++;
        sLI_Vars->sLevel.u32Current   += sLI_Vars->sLevel.i32Delta;
        sLI_Vars->sRed.u32Current     += sLI_Vars->sRed.i32Delta;
        sLI_Vars->sGreen.u32Current   += sLI_Vars->sGreen.i32Delta;
        sLI_Vars->sBlue.u32Current    += sLI_Vars->sBlue.i32Delta;
        sLI_Vars->sColTemp.u32Current += sLI_Vars->sColTemp.i32Delta;
        ip_updateDriver(sLI_Vars);
    }
}


void ip_updateDriver(tsLI_Vars* sLI_Vars)
{
	// Set output, ext. driver



	 vBULB_SetColour(sLI_Vars->sRed.u32Current   >> SCALE,
			         sLI_Vars->sGreen.u32Current >> SCALE,
			         sLI_Vars->sBlue.u32Current  >> SCALE);

	 vBULB_SetLevel(sLI_Vars->sLevel.u32Current  >> SCALE);

	 vBULB_SetColourTemperature(sLI_Vars->sColTemp.u32Current >> SCALE);
}


void ip_initVar(tsLI_Params *psLI_Params, uint32 u32NewTarget)
{
	psLI_Params->u32Target = u32NewTarget << SCALE;

	if (psLI_Params->u32Target < psLI_Params->u32Current)
	{
		psLI_Params->i32Delta = -(u32divu10(psLI_Params->u32Current - psLI_Params->u32Target));
	}
	else
	{
		psLI_Params->i32Delta = u32divu10(psLI_Params->u32Target - psLI_Params->u32Current);;
	}
	psLI_Params->u32Current += psLI_Params->i32Delta;
}


PRIVATE uint32  u32divu10(uint32 n)
{
    uint32 q, r;
	q = (n >> 1) + (n >> 2);
	q = q + (q >> 4);
	q = q + (q >> 8);
	q = q + (q >> 16);
	q = q >> 3;
	r = n - q*10;
	return q + ((r + 6) >> 4);
}

