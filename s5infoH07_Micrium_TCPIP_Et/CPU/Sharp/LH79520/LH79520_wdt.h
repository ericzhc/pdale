/**********************************************************************
 *  $Workfile:   LH79520_wdt.h  $
 *  $Revision:   1.6  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:52  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for LH79520 component:
 *
 *          Watchdog Timer
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/Processors/LH79520/LH79520_wdt.h-arc  $
 * 
 *    Rev 1.6   01 Apr 2002 09:32:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.5   Jan 31 2002 18:49:32   SuryanG
 * Corrected macro definitions WDT_CTRL_RSP_INT and
 * WDT_CTRL_RSP_RESET. Changed definition of 
 * WDT_CTRL_DISABLE.
 * 
 *    Rev 1.4   Jan 29 2002 14:20:12   SuryanG
 * Changed WDT_CTRL_RSP_FIQ to WDT_CTRL_RSP_INT.
 * Changed WDT_WD_NWDFIQ to WDT_WD_NWDINT.
 * 
 *    Rev 1.3   Jan 29 2002 13:59:10   SuryanG
 * Corrected WDT_CTRL_TOP macro.
 * 
 *    Rev 1.2   Jan 04 2002 12:12:28   BarnettH
 * Changed deprecated #ifndef
 * 
 *    Rev 1.1   Dec 05 2001 09:33:28   BarnettH
 * Removed unusable predefine
 * 
 *    Rev 1.0   Apr 04 2001 18:28:24   BarnettH
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

#if !defined LH79520_WDT_H
#define LH79520_WDT_H

#include "SMA_types.h"

/***********************************************************************
 * Watchdog Timer Module Register Structure
 **********************************************************************/
typedef struct 
{
    volatile UNS_32 wdctlr;     /* Control */ 
    volatile UNS_32 wdcntr;     /* Counter Reset */ 
    volatile UNS_32 wdtstr;     /* Status */ 
    volatile UNS_32 wdcnt0;     /* Counter Bits [7:0] */ 
    volatile UNS_32 wdcnt1;     /* Counter Bits [15:8] */ 
    volatile UNS_32 wdcnt2;     /* Counter Bits [23:16] */ 
    volatile UNS_32 wdcnt3;     /* Counter Bits [31:24] */ 
} WDTIMERREGS;

/**********************************************************************
 * Watchdog Timer Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * Watchdog Control Register Bit Fields
 *********************************************************************/ 
#define WDT_CTRL_ENABLE     _SBF(0,1)
#define WDT_CTRL_DISABLE    ~WDT_CTRL_ENABLE
#define WDT_CTRL_RSP_INT    _SBF(1,1)       
#define WDT_CTRL_RSP_RESET  ~WDT_CTRL_RSP_INT
#define WDT_CTRL_FRZ_ENABLE _BIT(3)
#define WDT_CTRL_TOP(n)     _SBF(4,((n)&_BITMASK(4)))

/**********************************************************************
 * Watchdog Counter Reset Register Bit Fields
 *********************************************************************/ 
#define WDT_WDCNTR          (0x1984)

/**********************************************************************
 * Watchdog Status Register Bit Fields
 *********************************************************************/ 
#define WDT_WD_NWDINT           _BIT(7)
#define WDT_WD_NWDRES           _BIT(6)
#define WDT_WD_RSP              _SBF(4,3)

#endif /* LH79520_WDT_H */ 
