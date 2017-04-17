/*
 * Interpolate.h
 *
 *  Created on: Apr 17, 2017
 *      Author: Peter Visser
 */

#ifndef INTERPOLATE_H_
#define INTERPOLATE_H_


#define INTPOINTS	(10)
#define SCALE 		(7)

typedef struct
{
	uint32 u32Current;
	uint32 u32Target;
	int32  i32Delta;
}tsLI_Params;

typedef struct
{
	tsLI_Params sLevel;
	tsLI_Params sRed;
	tsLI_Params sGreen;
	tsLI_Params sBlue;
	tsLI_Params sColTemp;
	uint32      u32PointsAdded;

}tsLI_Vars;


//PUBLIC void ip_createPoints(tsLI_Vars* sLI_Vars);
PUBLIC void ip_start(tsLI_Vars* sLI_Vars, uint32 u32Level, uint32 u32Red, uint32 u32Green, uint32 u32Blue, uint32 u32ColTemp);
PUBLIC void ip_stop(tsLI_Vars* sLI_Vars);
PUBLIC void ip_createPoints(tsLI_Vars* sLI_Vars);
PUBLIC void ip_setCurrentValues(tsLI_Vars* sLI_Vars, uint32 u32Level, uint32 u32Red, uint32 u32Green, uint32 u32Blue, uint32 u32ColTemp);

#endif /* INTERPOLATE_H_ */
