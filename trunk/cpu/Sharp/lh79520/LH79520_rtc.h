/**********************************************************************
 *  $Workfile:   LH79520_rtc.h  $
 *  $Revision:   1.2  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:50  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          Real Time Clock
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_rtc.h-arc  $  
 * 
 *    Rev 1.2   01 Apr 2002 09:32:50   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Jan 04 2002 10:42:54   BarnettH
 * Changed deprecated #ifndef
 * Removed union in Register Structure.
 * 
 *    Rev 1.0   Dec 05 2001 09:38:58   BarnettH
 * Initial revision.
 * 
 * SHARP MICROELECTRONICS OF THE AMERICAS MAKES NO REPRESENTATION
 * OR WARRANTIES WITH RESPECT TO THE PERFORMANCE OF THIS SOFTWARE,
 * AND SPECIFICALLY DISCLAIMS ANY RESPONSIBILITY FOR ANY DAMAGES, 
 * SPECIAL OR CONSEQUENTIAL, CONNECTED WITH THE USE OF THIS SOFTWARE.
 *
 * SHARP MICROELECTRONICS OF THE AMERICAS PROVIDES THIS SOFTWARE SOLELY 
 * FOR THE PURPOSE OF SOFTWARE DEVELOPMENT INCORPORATING THE USE OF A 
 * SHARP MICROCONTROLLER OR SYSTEM-ON-CHIP PRODUCT. USE OF THIS SOURCE
 * FILE IMPLIES ACCEPTANCE OF THESE CONDITIONS.
 *
 *  COPYRIGHT (C) 2001 SHARP MICROELECTRONICS OF THE AMERICAS, INC.
 *      CAMAS, WA
 *********************************************************************/

#if !defined LH79520_RTC_H
#define LH79520_RTC_H

#include "SMA_types.h"

/**********************************************************************
 * Real Time Clock Module Register Structure
 *********************************************************************/ 
typedef struct 
{
    volatile UNS_32 dr;         /* Data */ 
    volatile UNS_32 mr;         /* Match */ 
    volatile UNS_32 stat_eoi;   /* Interrupt Status/Interrupt Clear */ 
    volatile UNS_32 clr;        /* Counter Load */ 
    volatile UNS_32 cr;         /* Control */ 
} RTCREGS;

/**********************************************************************
 * Real Time Clock Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * RTC Status and Interrupt Clear Register (stat, eoi) Bit Fields
 *********************************************************************/ 
#define RTC_RTCINTR     _BIT(0)

/**********************************************************************
 * RTC Control Register (cr) Bit Fields
 *********************************************************************/ 
#define RTC_CR_MIE      _BIT(0)

#endif /* LH79520_RTC_H */ 
