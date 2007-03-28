/***********************************************************************
 * $Workfile:   SMA_sevenseg_driver.h  $
 * $Revision:   1.3  $
 * $Author:   BarnettH  $
 * $Date:   Feb 13 2002 15:33:18  $
 *
 * Project: General Utilities
 *
 * Description:
 *      Include file for utilities to drive seven-segment LED's
 *
 * Notes:
 *
 * Revision History:
 * $Log:   P:/PVCS6_6/archives/SOC/include/util/SMA_sevenseg_driver.h-arc  $
 * 
 *    Rev 1.3   Feb 13 2002 15:33:18   BarnettH
 * Changed file name and guard constant
 * Added include file SMA_types.h
 * Changed function names to comply with standard.
 * 
 *    Rev 1.2   Feb 04 2002 16:04:26   BarnettH
 * Removed all hard tabs.
 * 
 *    Rev 1.1   Jan 04 2002 09:57:44   BarnettH
 * Changed types to SMA_types.
 * 
 *    Rev 1.0   Oct 29 2001 13:31:50   BarnettH
 * Initial revision.
 * 
 * COPYRIGHT (C) 2001 SHARP MICROELECTRONICS OF THE AMERICAS, INC.
 *      CAMAS, WA 
 **********************************************************************/

#ifndef SMA_SEVENSEG_DRIVER_H
#define SMA_SEVENSEG_DRIVER_H

#include "SMA_types.h"

#define DP_OFF  1
#define DP_ON   0

void util_set_sevenseg_hexval (INT_32 val, UNS_8 dp);
void util_set_sevenseg_blank (void);

#endif /* SMA_SEVENSEG_DRIVER_H */ 
