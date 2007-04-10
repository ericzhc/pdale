/**********************************************************************
 *  $Workfile:   LH79520_pwm.h  $
 *  $Revision:   1.2  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:50  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for Sharp IP component:
 *
 *          Pulse Width Modulator (configurable dual-output)
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_pwm.h-arc  $
 * 
 *    Rev 1.2   01 Apr 2002 09:32:50   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Jan 04 2002 10:35:54   BarnettH
 * Changed deprecated #ifndef
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

#if !defined LH79520_PWM_H
#define LH79520_PWM_H

#include "SMA_types.h"

/**********************************************************************
 * Pulse Width Modulator Register Structure
 *********************************************************************/ 
typedef struct 
{
    volatile UNS_32 tc;     /* Terminal Count */ 
    volatile UNS_32 dc;     /* Duty Cycle */ 
    volatile UNS_32 en;     /* Enable */ 
    volatile UNS_32 inv;    /* Invert */ 
    volatile UNS_32 sync;   /* Synchronous */ 
    volatile UNS_32 reserved[3];
} PWMXREGS;

typedef struct 
{
    PWMXREGS    pwm0;
    PWMXREGS    pwm1;
} PWMREGS;

/**********************************************************************
 * Pulse Width Modulator Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * Enable Register Bit Fields
 *********************************************************************/ 
#define PWM_EN_ENABLE       _BIT(0)

/**********************************************************************
 * Invert Register Bit Fields
 *********************************************************************/ 
#define PWM_INV_INVERT      _BIT(0)

/**********************************************************************
 * Synchronous Mode Register Bit Fields
 *********************************************************************/ 
#define PWM_SYNC_NORMAL     _SBF(0,0)
#define PWM_SYNC_SYNC       _SBF(0,1)
#define PWM_SYNC_SOURCE0    _SBF(1,0)
#define PWM_SYNC_SOURCE1    _SBF(1,1)

#endif /* LH79520_PWM_H */ 
