/**********************************************************************
 *  $Workfile:   LH79520_sdrc.h  $
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
 *          Synchronous Dynamic RAM Memory Controller
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_sdrc.h-arc  $
 * 
 *    Rev 1.2   01 Apr 2002 09:32:50   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Jan 04 2002 10:44:34   BarnettH
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

#if !defined LH79520_SDRC_H
#define LH79520_SDRC_H

#include "SMA_types.h"

/*********************************************************************** 
 * SDRAM Controller Module Register Structure
 **********************************************************************/

typedef struct 
{
    volatile UNS_32 config0;    /* Configuration 0 */ 
    volatile UNS_32 config1;    /* Configuration 1 */ 
    volatile UNS_32 reftimer;   /* Refresh Timer */ 
    volatile UNS_32 wbtimeout;  /* Write Buffer Timeout */ 
} SDRAMREGS;

/*********************************************************************** 
 * SDRAM Controller Configuration Register 0 Bit Field constants
 **********************************************************************/ 
#define SDRAM_A_AUTO    _SBF(24,1)  /* Auto Pre-charge */ 
#define SDRAM_A_NOAUTO  _SBF(24,0)  /* No Auto Pre-charge */ 
#define SDRAM_C_CONT    _SBF(18,1)  /* Clock Enable Continuous */ 
#define SDRAM_C_IDLE    _SBF(18,0)  /* Clock Enable Idle deasserted */ 
#define SDRAM_CLAT1     _SBF(20,1)  /* CAS latency 1 */ 
#define SDRAM_CLAT2     _SBF(20,2)  /* CAS latency 2 */ 
#define SDRAM_CLAT3     _SBF(20,3)  /* CAS latency 3 */ 
#define SDRAM_CS0_256M  _BIT(1)     /* nCSOut[0] is 256MBit device */ 
#define SDRAM_CS0_2BANK _SBF(3,0)   /* nCSOut[0] is 2 bank device*/
#define SDRAM_CS0_4BANK _SBF(3,1)   /* nCSOut[0] is 4 bank device*/
#define SDRAM_CS0_X16   _SBF(2,0)   /* nCSOut[0] is x16 device*/
#define SDRAM_CS0_X32   _SBF(2,0)   /* nCSOut[0] is x32 device*/
#define SDRAM_CS0_X8    _SBF(2,1)   /* nCSOut[0] is x8 device*/
#define SDRAM_CS1_256M  _BIT(5)     /* nCSOut[1] is 256MBit device */ 
#define SDRAM_CS1_2BANK _SBF(7,0)   /* nCSOut[1] is 2 bank device*/
#define SDRAM_CS1_4BANK _SBF(7,1)   /* nCSOut[1] is 4 bank device*/
#define SDRAM_CS1_X16   _SBF(6,0)   /* nCSOut[1] is x16 device*/
#define SDRAM_CS1_X32   _SBF(6,0)   /* nCSOut[1] is x32 device*/
#define SDRAM_CS1_X8    _SBF(6,1)   /* nCSOut[1] is x8 device*/
#define SDRAM_CS2_256M  _BIT(9)     /* nCSOut[2] is 256MBit device */ 
#define SDRAM_CS2_2BANK _SBF(11,0)  /* nCSOut[2] is 2 bank device*/
#define SDRAM_CS2_4BANK _SBF(11,1)  /* nCSOut[2] is 4 bank device*/
#define SDRAM_CS2_X16   _SBF(10,0)  /* nCSOut[2] is x16 device*/
#define SDRAM_CS2_X32   _SBF(10,0)  /* nCSOut[2] is x32 device*/
#define SDRAM_CS2_X8    _SBF(10,1)  /* nCSOut[2] is x8 device*/
#define SDRAM_CS3_256M  _BIT(13)    /* nCSOut[3] is 256MBit device */ 
#define SDRAM_CS3_2BANK _SBF(15,0)  /* nCSOut[3] is 2 bank device*/
#define SDRAM_CS3_4BANK _SBF(15,1)  /* nCSOut[3] is 4 bank device*/
#define SDRAM_CS3_X16   _SBF(14,0)  /* nCSOut[3] is x16 device*/
#define SDRAM_CS3_X32   _SBF(14,0)  /* nCSOut[3] is x32 device*/
#define SDRAM_CS3_X8    _SBF(14,1)  /* nCSOut[3] is x8 device*/
#define SDRAM_E_CONT    _SBF(17,0)  /* Clockout Continuous */ 
#define SDRAM_E_IDLE    _SBF(17,1)  /* Clockout stops on Idle */ 
#define SDRAM_RCLAT1    _SBF(22,1)  /* RAS to CAS latency 1 */ 
#define SDRAM_RCLAT2    _SBF(22,2)  /* RAS to CAS latency 2 */ 
#define SDRAM_RCLAT3    _SBF(22,3)  /* RAS to CAS latency 3 */ 
#define SDRAM_EXTBUS16  _SBF(19,1)  /* External Bus Width 16 */ 
#define SDRAM_EXTBUS32  _SBF(19,0)  /* External Bus Width 32 */ 

/*********************************************************************** 
 * SDRAM Controller Configuration Register 1 Bit Field constants
 **********************************************************************/ 
#define SDRAM_INIT_NORMAL   _SBF(0,0)   /* Normal Operation */ 
#define SDRAM_INIT_PALL     _SBF(0,1)   /* Init Control PALL */ 
#define SDRAM_INIT_MODE     _SBF(0,2)   /* Init Control MODE */ 
#define SDRAM_INIT_NOP      _SBF(0,3)   /* Init Control NOP */ 
#define SDRAM_STATUS        _BIT(5)     /* SDRAM Engine Status */ 
#define SDRAM_WB_DISABLE    _SBF(3,0)   /* Write Buffer Disable */ 
#define SDRAM_WB_ENABLE     _SBF(3,1)   /* Write Buffer Enable */ 
#define SDRAM_RB_DISABLE    _SBF(2,0)   /* Read Buffer Disable */ 
#define SDRAM_RB_ENABLE     _SBF(2,1)   /* Read Buffer Enable */ 

/*********************************************************************** 
 * SDRAM Controller Refresh Timer Register Bit Field
 **********************************************************************/ 
/* Set Refresh Timer */ 
#define SDRAM_SET_REFRESH(n)    ((n)&_BITMASK(16))

/*********************************************************************** 
 * SDRAM Controller Write Buffer Timeout Register Bit Field
 **********************************************************************/ 
/* Set Write Buffer Timeout */ 
#define SDRAM_SET_WBTIMEOUT(n)  ((n)&_BITMASK(16))

#endif /* LH79520_SDRC_H */ 
