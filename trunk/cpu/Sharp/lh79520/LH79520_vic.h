/**********************************************************************
 *  $Workfile:   LH79520_vic.h  $
 *  $Revision:   1.3  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:52  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for LH79520 component:
 *
 *          Vectored Interrupt Controller
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_vic.h-arc  $
 * 
 *    Rev 1.3   01 Apr 2002 09:32:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.2   Jan 28 2002 16:20:42   KovitzP
 * Corrected interrupt source defines to match final chip source
 * list. Made source list an enum instead of a series of #defines.
 * 
 *    Rev 1.1   Jan 04 2002 12:11:24   BarnettH
 * Changed deprecated #ifndef
 * Deleted invalid structure elements and invalid structure.
 * Added VIC_INT_TYPE_xxx.
 * Deleted unnecessary manifest constants.
 * Added VIC Vectors and VIC Interrupt Sources.
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

#if !defined LH79520_VIC_H
#define LH79520_VIC_H

#include "SMA_types.h"

/*
 * Vectored Interrupt Controller Module Register Structure
 */ 
typedef struct 
{
    volatile UNS_32 irqstatus;
    volatile UNS_32 fiqstatus;
    volatile UNS_32 rawintr;
    volatile UNS_32 intselect;
    volatile UNS_32 intenable;
    volatile UNS_32 intenclear;
    volatile UNS_32 softint;
    volatile UNS_32 softintclear;
    volatile UNS_32 reserved1[4];
    volatile UNS_32 vectoraddr;     
    volatile UNS_32 defvectaddr;
    volatile UNS_32 reserved2[50];
    volatile UNS_32 vectaddr[16];   /* name arranged as array */ 
    volatile UNS_32 reserved3[48];
    volatile UNS_32 vectcntl[16];   /* name arranged as array */ 
    volatile UNS_32 reserved4[51];
    volatile UNS_32 itop1;
} VICREGS;

/**********************************************************************
 * Vectored Interrupt Controller Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * The bit fields of the following registers have implementation
 * specific meaning, and must be defined at the implementation level.
 *
 *      irqstatus   - VICIRQStatus
 *      fiqstatus   - VICFIQStatus
 *      rawintr     - VICRawIntr
 *      intselect   - VICIntSelect
 *      intenable   - VICIntEnable
 *      intenclear  - VICIntEnClear
 *      softint     - VICSoftInt
 *      softintclear- VICSoftIntClear
 *
 * The following definitions for these registers are generic,
 * i.e., they are implementation independent. They can be used to
 * create implementation specific macros.
 *********************************************************************/ 

/**********************************************************************
 * VIC Interrupt Select Register Bit Fields
 *********************************************************************/ 
/* The following can be OR'd with the IntSelect Register to select
 * an interrupt as FIQ. */ 
#define VIC_INTSELECT_FIQ(n)    _BIT((n) & _BITMASK(5))
/* The following can be AND'd with the IntSelect Register to select
 * an interrupt as IRQ. */ 
#define VIC_INTSELECT_IRQ(n)    ~(_BIT((n) & _BITMASK(5)))

#define VIC_INT_TYPE_IRQ    0
#define VIC_INT_TYPE_FIQ    1

/**********************************************************************
 * VIC Interrupt Enable, Interrupt Enable Clear Register Bit Fields
 * VIC Soft Interrupt, Soft Interrupt Clear Register Bit Fields
 *********************************************************************/ 
#define VIC_INT_ENABLE(n)       _BIT((n) & _BITMASK(5))
#define VIC_INT_CLEAR(n)        _BIT((n) & _BITMASK(5))

/**********************************************************************
 * VIC Vector Address Clear Register 
 *********************************************************************/ 
#define VIC_VECTORADDR_CLEAR    0

/**********************************************************************
 * VIC Vector Control Register Bit Fields
 *********************************************************************/ 

/* To revise a Vector Control Register, clear the register, then 
 * use the SELECT macro to associate a line and enable the vector 
 * with the same operation.
 * The ENABLE macro is provided for completeness.
 * Use this register to enable and disable the VECTOR feature;
 * use the intenable register to enable the interrupt
 * itself, and the intenclear register to clear the interrupt. */ 

#define VIC_VECTCNTL_SELECT(n)  (_SBF(0,((n) & _BITMASK(5))) | _BIT(5))
#define VIC_VECTCNTL_ENABLE     _BIT(5)

/**********************************************************************
 * Vectored Interrupt Controller Register ITOP1   - Test Output 1
 * (used for debugging)
 *********************************************************************/ 
#define VIC_ITOP1_FIQ         _BIT(6)
#define VIC_ITOP1_IRQ         _BIT(7)

/***********************************************************************
 * VIC Vectors
 **********************************************************************/
#define VIC_VECT_0          0
#define VIC_VECT_1          1
#define VIC_VECT_2          2
#define VIC_VECT_3          3
#define VIC_VECT_4          4
#define VIC_VECT_5          5
#define VIC_VECT_6          6
#define VIC_VECT_7          7
#define VIC_VECT_8          8
#define VIC_VECT_9          9
#define VIC_VECT_10         10
#define VIC_VECT_11         11
#define VIC_VECT_12         12
#define VIC_VECT_13         13
#define VIC_VECT_14         14
#define VIC_VECT_15         15
#define VIC_VECT_MAX        VIC_VECT_15
#define VIC_VECT_DEFAULT    ~(0)

/***********************************************************************
 * LH79520 VIC Interrupt Sources
 **********************************************************************/
typedef enum
{
   VIC_EXINT0,     /* external interrupt 0 */
   VIC_EXINT1,     /* external interrupt 1 */
   VIC_EXINT2,     /* external interrupt 2 */
   VIC_EXINT3,     /* external interrupt 3 */
   VIC_EXINT4,     /* external interrupt 4 */
   VIC_EXINT5,     /* external interrupt 5 */
   VIC_EXINT6,     /* external interrupt 6 */
   VIC_EXINT7,     /* external interrupt 7 */
   VIC_SPAREINT0,  /* spare interrupt 0 */
   VIC_COMRX,      /* debug channel receive interrupt */
   VIC_COMTX,      /* debug channel transmit interrupt */
   VIC_SSPRXTO,    /* SSP receive timeout interrupt */
   VIC_CLCD,       /* LCD controller combined interrupt */
   VIC_SSPTX,      /* SSP transmit interrupt */
   VIC_SSPRX,      /* SSP receive interrupt */
   VIC_SSPROR,     /* SSP receive overrun interrupt */
   VIC_SSPINT,     /* SSP combined interrupt */
   VIC_TIMER0,       /* counter-timer0 nterrupt */
   VIC_TIMER1,       /* counter-timer1 interrupt */
   VIC_TIMER2,       /* counter-timer2 interrupt */
   VIC_TIMER3,       /* counter-timer3 interrupt */
   VIC_UARTRX0,    /* UART0 Receive interrupt */
   VIC_UARTTX0,    /* UART0 Transmit interrupt */
   VIC_UARTINT0,   /* UART0 combined interrupt */
   VIC_UARTINT1,   /* UART1 combined interrupt */
   VIC_UARTINT2,   /* UART2 combined interrupt */
   VIC_DMA,        /* DMA combined interrupt */
   VIC_SPAREINT1,  /* spare interrupt 1 */
   VIC_SPAREINT2,  /* spare interrupt 2 */
   VIC_SPAREINT3,  /* spare interrupt 3 */
   VIC_RTCINT,     /* Real-time Clock interrupt 1 */
   VIC_WDTINT,     /* Watchdog timer interrupt 1 */
   VIC_BAD_SOURCE  /* Invalid source; drivers return this on error */
} VIC_SOURCE;

#endif /* LH79520_VIC_H */ 
