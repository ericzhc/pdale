/**********************************************************************
 * $Workfile:   ARM_arch.h  $
 * $Revision:   1.2  $
 * $Author:   kovitzp  $
 * $Date:   01 Apr 2002 09:32:06  $
 *
 * Project: General Utilities
 *
 * Description:
 *      This file contains constant and macro definitions peculiar
 *      to the ARM architecture. 
 *
 * Revision History:
 * $Log:   P:/PVCS6_6/archives/SOC/include/util/ARM_arch.h-arc  $
 * 
 *    Rev 1.2   01 Apr 2002 09:32:06   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Feb 11 2002 17:48:14   BarnettH
 * Added Condition Code Flag bits
 * Added IRQ, FIQ mask bits
 * Added Thumb State bit
 * Added ARM Mode values, and Mode Test macro.
 * 
 *    Rev 1.0   Jan 31 2002 17:19:22   SuryanG
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
 * COPYRIGHT (C) 2002 SHARP MICROELECTRONICS OF THE AMERICAS, INC.
 *      CAMAS, WA
 *********************************************************************/
#if !defined ARM_ARCH_H
#define ARM_ARCH_H

#include "SMA_types.h"

/**********************************************************************
 * ARM Hard Vector Address Locations
 *********************************************************************/
#define ARM_RESET_VEC   0x00
#define ARM_UNDEF_VEC   0x04
#define ARM_SWI_VEC     0x08
#define ARM_IABORT_VEC  0x0C
#define ARM_DABORT_VEC  0x10
#define ARM_IRQ_VEC     0x18
#define ARM_FIQ_VEC     0x1C

/**********************************************************************
 * ARM Current and Saved Processor Status Register Bits (xPSR)
 *********************************************************************/
 // ARM Condition Code Flag Bits (xPSR bits [31:27]) 
#define ARM_CCFLG_N     _BIT(31)
#define ARM_CCFLG_Z     _BIT(30)
#define ARM_CCFLG_C     _BIT(29)
#define ARM_CCFLG_V     _BIT(28)
#define ARM_CCFLG_Q     _BIT(27)

 // ARM Interrupt Disable Bits (xPSR bits [7:6]) 
#define ARM_IRQ         _BIT(7)
#define ARM_FIQ         _BIT(6)

 // ARM Thumb State Bit (xPSR bit [5]) 
#define ARM_THUMB       _BIT(5)

 // ARM Processor Mode Values (xPSR bits [4:0]) 
 // Use ARM_MODE macro and constants to test ARM mode
 // Example, where tmp has xPSR value:
 // if (ARM_MODE(tmp, ARM_MODE_IRQ))
 //     statement;
#define ARM_MODE(m,n)   (((m) & _BITMASK(5)) == (n))
#define ARM_MODE_USR    0x10
#define ARM_MODE_FIQ    0x11
#define ARM_MODE_IRQ    0x12
#define ARM_MODE_SVC    0x13
#define ARM_MODE_ABT    0x17
#define ARM_MODE_UND    0x1B
#define ARM_MODE_SYS    0x1F

#endif /* ARM_ARCH_H */
