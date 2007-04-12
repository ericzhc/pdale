/**********************************************************************
 *  $Workfile:   LH79520_smc.h  $
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
 *          Static Memory Controller
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_smc.h-arc  $
 * 
 *    Rev 1.3   01 Apr 2002 09:32:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.2   Feb 08 2002 13:58:16   MaysR
 * Removed static bit field constants from file as per TeamTrack item isis-00047.
 *
 *    Rev 1.1   Jan 04 2002 10:46:18   BarnettH
 * Changed deprecated #ifndef.
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

#if !defined LH79520_SMC_H
#define LH79520_SMC_H

#include "SMA_types.h"

/***********************************************************************
 * Static Memory Controller Module Register Structure
 **********************************************************************/

typedef struct 
{
    volatile UNS_32 bcr0;    /* Configuration for bank 0 */
    volatile UNS_32 bcr1;    /* Configuration for bank 1 */
    volatile UNS_32 bcr2;    /* Configuration for bank 2 */
    volatile UNS_32 bcr3;    /* Configuration for bank 3 */
    volatile UNS_32 bcr4;    /* Configuration for bank 4 */
    volatile UNS_32 bcr5;    /* Configuration for bank 5 */
    volatile UNS_32 bcr6;    /* Configuration for bank 6 */
    volatile UNS_32 bcr7;    /* Configuration for bank 7 */
} SMCREGS;

/***********************************************************************
 * Static Memory Controller Bit Field constants
 **********************************************************************/
#define IDCY(n)     _SBF(0,((n)&_BITMASK(4)))  /* Idle Cycle Time */
#define WST1(n)     _SBF(5,((n)&_BITMASK(5)))  /* Wait State 1 */
#define RBLE(n)     _SBF(10,((n)&_BITMASK(1))) /*Read Byte Lane Enable*/
#define WST2(n)     _SBF(11,((n)&_BITMASK(5))) /* Wait State 2 */
#define BUSERR      _BIT(24)            /* Bus Transfer Error Flag */
#define WPERR       _BIT(25)            /* Write Protect Error Flag */
#define WP          _BIT(26)            /* Write Protect */
#define BM          _BIT(27)            /* Burst Mode */
#define MW8         _SBF(28,0)          /* Memory width 8 bits */
#define MW16        _SBF(28,1)          /* Memory width 16 bits */
#define MW32        _SBF(28,2)          /* Memory width 32 bits */

#endif /* LH79520_SMC_H */
