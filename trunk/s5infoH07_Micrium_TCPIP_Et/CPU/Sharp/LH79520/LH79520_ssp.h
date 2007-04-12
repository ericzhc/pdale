/**********************************************************************
 *  $Workfile:   LH79520_ssp.h  $
 *  $Revision:   1.4  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:52  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          Synchronous Serial Port 
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_ssp.h-arc  $
 * 
 *    Rev 1.4   01 Apr 2002 09:32:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.3   Feb 13 2002 14:01:48   BarnettH
 * Changed comment on SSP_CR0_SPH
 * 
 *    Rev 1.2   Jan 28 2002 16:11:48   KovitzP
 * Added rxto register back in (it was previously defined as
 * one of the reserved registers).
 * 
 *    Rev 1.1   Jan 04 2002 10:47:18   BarnettH
 * Changed deprecated #ifndef
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

#if !defined LH79520_SSP_H
#define LH79520_SSP_H

#include "SMA_types.h"

/**********************************************************************
 * Synchronous Serial Port Register Structure
 *********************************************************************/ 
typedef struct 
{
    volatile UNS_32 cr0;
    volatile UNS_32 cr1;
    volatile UNS_32 dr;
    volatile UNS_32 sr;
    volatile UNS_32 cpsr;
    union 
    {
        volatile UNS_32 iir;
        volatile UNS_32 icr;
    } u;
    volatile UNS_32 rxto;
} SSPREGS;

/**********************************************************************
 * Synchronous Serial Port Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * SSP Control 0 Register Bit Fields
 *********************************************************************/ 
/* Valid range for argument to SSP_CR0_DSS(n) is [4-16] */ 
#define SSP_CR0_DSS(n)  _SBF(0,(n)-1)   /* Data Size Select */ 
#define SSP_CR0_FRF_MOT _SBF(4,0)   /* Motorola SPI frame */ 
#define SSP_CR0_FRF_TI  _SBF(4,1)   /* TI synchronous serial frame */ 
#define SSP_CR0_FRF_NS  _SBF(4,2)   /* National Microwire frame */ 
#define SSP_CR0_SPO     _BIT(6)     /* SPI Polarity */ 
#define SSP_CR0_SPH     _BIT(7)     /* SPI Phase */ 
#define SSP_CR0_SCR(n)  _SBF(8,(n)) /* Serial Clock Rate */ 

/**********************************************************************
 * SSP Control 0 Register Bit Fields
 *********************************************************************/ 
#define SSP_CR1_RIE     _BIT(0)     /* RX FIFO interrupt enable */ 
#define SSP_CR1_TIE     _BIT(1)     /* TX FIFO interrupt enable */
#define SSP_CR1_RORIE   _BIT(2)     /* RX FIFO overrun int. enable */ 
#define SSP_CR1_LBM     _BIT(3)     /* Loop back mode */ 
#define SSP_CR1_SSE     _BIT(4)     /* Synchronous serial port enable */ 

/**********************************************************************
 * SSP Status Register Bit Fields
 *********************************************************************/ 
#define SSP_SR_TFE      _BIT(0)     /* TX FIFO Empty */ 
#define SSP_SR_TNF      _BIT(1)     /* TX FIFO not full */
#define SSP_SR_RNE      _BIT(2)     /* RX FIFO not empty */ 
#define SSP_SR_RFF      _BIT(3)     /* RX FIFO full */ 
#define SSP_SR_BSY      _BIT(4)     /* Busy flag */ 

/**********************************************************************
 * SSP Clock Prescale Divisor Register Bit Fields
 *********************************************************************/ 
#define SSP_CPSR_CPDVSR(n) _SBF(0,(n)&0xFE) /* Clock prescale divisor */

/**********************************************************************
 * SSP Interrupt Identification / Interrupt Clear Register Bit Fields
 * Note: Reference conflicts on the definition of these bits
 * and the usage of the registers. Verify before using these
 * definitions.
 *********************************************************************/ 
#define SSP_IIR_RIS     _BIT(0)     /* TX FIFO Empty */ 
#define SSP_IIR_TIS     _BIT(1)     /* TX FIFO not full */
#define SSP_IIR_RORIS   _BIT(2)     /* RX FIFO overrun int. status */ 

#endif /* LH79520_SSP_H */ 
