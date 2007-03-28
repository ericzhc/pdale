/**********************************************************************
 *  $Workfile:   LH79520_lcdicp.h  $
 *  $Revision:   1.2  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:24  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          LCD Interface Conversion Peripheral
 *
 *      The LCDICP Interface can be used to manipulate signals from
 *      the LCD Controller to provide control of an HR-TFT display.
 *      The LCDICP is designed to be used in conjunction with the
 *      CLCD Controller (CLCDC) (see LH79520_clcdc.h).  The CLCDC
 *      supplies the LCDICP with the standard TFT output, which is
 *      manipulated to provide control of an HR-TFT display.
 *      See reference.
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_lcdicp.h-arc  $
 * 
 *    Rev 1.2   01 Apr 2002 09:32:24   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Jan 04 2002 10:13:38   BarnettH
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

#if !defined LH79520_LCDICP_H
#define LH79520_LCDICP_H

#include "SMA_types.h"

/**********************************************************************
 * Reset, Clock, and Power Controller Register Structure
 *********************************************************************/ 
typedef struct 
{
    volatile UNS_32 setup;      /* Setup */ 
    volatile UNS_32 control;    /* Control */ 
    volatile UNS_32 timing1;    /* HR-TFT Timing 1 */ 
    volatile UNS_32 timing2;    /* HR-TFT Timing 2 */ 
} LCDICPREGS;

/**********************************************************************
 * LCD Interface Conversion Peripheral (LCDICP) Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * Setup Register Bit Fields
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than zero.
 *********************************************************************/ 
#define LCDICP_SETUP_MODE_BYPASS    _SBF(0,0)
#define LCDICP_SETUP_MODE_HRTFT     _SBF(0,1)
#define LCDICP_SETUP_MODE_DMTN      _SBF(0,2)
#define LCDICP_SETUP_HORIZ_REVERSE  _SBF(2,0)
#define LCDICP_SETUP_HORIZ_NORMAL   _SBF(2,1)
#define LCDICP_SETUP_VERT_REVERSE   _SBF(3,0)
#define LCDICP_SETUP_VERT_NORMAL    _SBF(3,1)
/* Calculates bit field value from actual pixels per line */ 
#define LCDICP_SETUP_PPL(n)         _SBF(4,((n)-1)&_BITMASK(9))

/**********************************************************************
 * Control Register Bit Fields
 *********************************************************************/ 
#define LCDICP_CONTROL_SPSEN        _BIT(0)
#define LCDICP_CONTROL_CLSEN        _BIT(1)
#define LCDICP_CONTROL_UBLEN        _BIT(2)
#define LCDICP_CONTROL_DISP         _BIT(3)
#define LCDICP_CONTROL_EN0          _BIT(4)
#define LCDICP_CONTROL_EN1          _BIT(5)
#define LCDICP_CONTROL_EN2          _BIT(6)
#define LCDICP_CONTROL_EN3          _BIT(7)

/**********************************************************************
 * Timing 1 Register Bit Fields
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than zero.
 *********************************************************************/ 
#define LCDICP_TIMING1_LPDEL(n)         _SBF(0,((n)-1)&_BITMASK(4))
#define LCDICP_TIMING1_REVDEL(n)        _SBF(4,((n)-1)&_BITMASK(4))
#define LCDICP_TIMING1_PSDEL_CLSDEL(n)  _SBF(8,((n)-1)&_BITMASK(4))

/**********************************************************************
 * Timing 2 Register Bit Fields
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than zero.
 *********************************************************************/ 
#define LCDICP_TIMING2_PSDEL2_CLSDEL2(n)  _SBF(0,((n)-1)&_BITMASK(9))
#define LCDICP_TIMING2_SPLVALUE(n)        _SBF(9,((n)-1)&_BITMASK(7))

#endif /* LH79520_LCDICP_H */ 
