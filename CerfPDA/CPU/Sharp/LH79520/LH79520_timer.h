/**********************************************************************
 *  $Workfile:   LH79520_timer.h  $
 *  $Revision:   1.3  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:52  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          Timers
 *
 *  Usage:  TIMERREG
 *
 *      Timer Module Register Structure (single) TIMERREG
 *
 *      Use structure TIMERREG to access an individual timer.
 *
 *      Usage Example:
 *
 *      [These manifest constants are defined in the LH79520_map.h file.]
 *
 *      #define TIMER0REGS_BASE     (0xFFFC4000)
 *      #define TIMER1REGS_BASE     (0xFFFC4020)
 *      #define TIMER2REGS_BASE     (0xFFFC5000)
 *      #define TIMER3REGS_BASE     (0xFFFC5020)
 *
 *      #define TIMER0 ((TIMERREG *)(TIMER0REGS_BASE))
 *      #define TIMER1 ((TIMERREG *)(TIMER1REGS_BASE))
 *      #define TIMER2 ((TIMERREG *)(TIMER2REGS_BASE))
 *      #define TIMER3 ((TIMERREG *)(TIMER3REGS_BASE))
 *      
 *      Alternatively: 
 *
 *      #define TIMER1REGS_BASE     (0xFFFC4000)
 *      #define TIMER2REGS_BASE     (0xFFFC5000)
 *
 *      #define TIMER0 ((TIMERREG *)(TIMER1REGS_BASE))
 *      #define TIMER1 ((TIMERREG *)(TIMER1REGS_BASE + 0x20))
 *      #define TIMER2 ((TIMERREG *)(TIMER2REGS_BASE))
 *      #define TIMER3 ((TIMERREG *)(TIMER2REGS_BASE + 0x20))
 *
 *
 *
 *      User code:
 *      {
 *      TIMER0->Clear = 0;
 *      TIMER0->Load = startval;
 *      TIMER0->Control = TMRCTRL_ENABLE | TMRCTRL_MODE_PERIODIC;
 *          *
 *          *
 *          *
 *      TIMER2->Clear = 0;
 *      TIMER2->Load = startval;
 *      TIMER2->Control = TMRCTRL_ENABLE | TMRCTRL_MODE_FREERUN;
 *      }
 *
 *  Note (1): If data other than the manifest constants in this file
 *          are used to set the timer control register, note that all
 *          bits other than those defined must be written as zero.
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_timer.h-arc  $
 * 
 *    Rev 1.3   01 Apr 2002 09:32:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.2   Jan 04 2002 12:05:40   BarnettH
 * Changed deprecated #ifndef
 * Changed structure element names to lower-case
 * 
 *    Rev 1.1   Dec 13 2001 17:01:08   BarnettH
 * Simplified structures.
 * Simplified remaining structure member names.
 * 
 *    Rev 1.0   Dec 05 2001 09:39:00   BarnettH
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

#if !defined LH79520_TIMER_H
#define LH79520_TIMER_H

#include "SMA_types.h"

/***********************************************************************
 * Timer Module Register Structure
 **********************************************************************/
typedef struct 
{
    volatile UNS_32 load;
    volatile UNS_32 value;
    volatile UNS_32 control;
    volatile UNS_32 clear;
} TIMERREG;

/***********************************************************************
 * Timer Control Register Bit Field constants
 * All other bits in the Timer Control Register must be written as
 * zero
 **********************************************************************/
#define TMRCTRL_ENABLE          _SBF(7,1)
#define TMRCTRL_DISABLE         _SBF(7,0)
#define TMRCTRL_MODE_PERIODIC   _SBF(6,1)
#define TMRCTRL_MODE_FREERUN    _SBF(6,0)
#define TMRCTRL_CASCADE_ENABLE  _SBF(4,1)
#define TMRCTRL_CASCADE_DISABLE _SBF(4,0)
#define TMRCTRL_PRESCALE1       _SBF(2,0)
#define TMRCTRL_PRESCALE16      _SBF(2,1)
#define TMRCTRL_PRESCALE256     _SBF(2,2)

#endif /* LH79520_TIMER_H */ 
