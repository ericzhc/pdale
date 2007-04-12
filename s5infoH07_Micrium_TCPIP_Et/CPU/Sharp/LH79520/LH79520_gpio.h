/**********************************************************************
 *  $Workfile:   LH79520_gpio.h  $
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
 *          General Purpose Input/Output
 *
 *      Each GPIO Module has two GPIO (digital IO) ports which are
 *      designated PORTA and PORTB.
 *
 *      Multiple instances of a GPIO module, and thus port pairs,
 *      may be implemented in a single SOC.
 *
 *      Each port has eight bits, PORTx[7:0].
 *
 *      Each port has two 8-bit registers associated with it:
 *          GPIOPxDR -  Data register   (dr)
 *          GPIOPxDDR - Data Direction register     (ddr)
 *
 *      The specific SOC will have its own unique name for the port.
 *      Each port pair will have its own unique base address for the
 *      port pair.
 *
 *      This include file is designed to permit the definition of 
 *      pointers in the SOC map include file to distinct 8-bit ports
 *      using the name designation is suitable for the SOC
 *      implementation.
 *
 *      Example:
 *      A map file that includes this file should specify a
 *      base address for each GPIO module, e.g.:
 *
 *      #define GPIO0_BASE  (0xFFFDF000)
 *      #define GPIO1_BASE  (0xFFFDE000)
 *          *
 *          *
 *          *
 *
 *      The SOC map file will use these base addresses to define
 *      pointers to GPIO port A, B, C, D, E, ... thusly:
 *
 *      #define GPIOPA  ((volatile GPIOAREGS *)(GPIO0_BASE))
 *      #define GPIOPB  ((volatile GPIOBREGS *)(GPIO0_BASE))
 *      #define GPIOPC  ((volatile GPIOAREGS *)(GPIO1_BASE))
 *      #define GPIOPD  ((volatile GPIOBREGS *)(GPIO1_BASE))
 *      #define GPIOPE  ((volatile GPIOAREGS *)(GPIO2_BASE))
 *          *
 *          *
 *          *
 *
 *      Example usage of these definitions in user code for Port C:
 *
 *          UNS_32 data;
 *
 *          GPIOC->ddr = 0xF;           sets bits [7:4] as outputs
 *                                      and bits [3:0] as inputs
 *          GPIOC->dr  = 0xF0;          sets bits [7:4] to "1"
 *
 *          data = GPIOC->dr;           sets data to the value of
 *                                      data register
 * 
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_gpio.h-arc  $
 * 
 *    Rev 1.2   01 Apr 2002 09:32:24   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Jan 04 2002 10:09:00   BarnettH
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

 #if !defined LH79520_GPIO_H
 #define LH79520_GPIO_H

#include "SMA_types.h"

/* GPIO Register Structures */ 
typedef struct 
{
    volatile UNS_32   dr;
    volatile UNS_32   reserveda1;
    volatile UNS_32   ddr;
    volatile UNS_32   reserveda2;
} GPIOPAREGS;

typedef struct 
{
    volatile UNS_32   reservedb1;
    volatile UNS_32   dr;
    volatile UNS_32   reservedb2;
    volatile UNS_32   ddr;
} GPIOPBREGS;

/*
 * The names and usage of the bit fields in these registers is
 * implementation specific, so no bit field constants are defined.
 */ 

#endif /* LH79520_GPIO_H */ 
