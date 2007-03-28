/**********************************************************************
 *  $Workfile:   LH79520_iocon.h  $
 *  $Revision:   1.6  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:24  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for LH79520 component:
 *
 *          I/O Control Peripheral
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/Processors/LH79520/LH79520_iocon.h-arc  $
 * 
 *    Rev 1.6   01 Apr 2002 09:32:24   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.5   Jan 04 2002 10:10:58   BarnettH
 * Changed deprecated #ifndef
 * Changed structure member names to lower-case.
 * 
 *    Rev 1.4   Dec 05 2001 09:31:02   BarnettH
 * Moved _BIT and _SBF definition to SMA_types.h
 * 
 *    Rev 1.3   Dec 04 2001 16:16:42   BarnettH
 * Removed unusable pre-define
 * Changed types to SMA_types
 * Changed UARTMUX constant symbols to agree with documentation.
 * 
 *    Rev 1.2   Aug 28 2001 14:16:20   KovitzP
 * updated LCDMUX constant to match rev D of the TRM
 * 
 *    Rev 1.1   Jul 26 2001 10:01:32   BarnettH
 * Definition of MEMMUX_MIDQM30 changed to set bits [1:0]
 * 
 *    Rev 1.0   Apr 03 2001 17:52:34   BarnettH
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

#if !defined LH79520_IOCON_H
#define LH79520_IOCON_H

#include "SMA_types.h"

/***********************************************************************
 * IO Configuration Block Structure
 **********************************************************************/

typedef struct 
{
    volatile UNS_32   memmux;
    volatile UNS_32   lcdmux;
    volatile UNS_32   miscmux;
    volatile UNS_32   dmamux;
    volatile UNS_32   uartmux;
    volatile UNS_32   ssimux;
    volatile UNS_32   scratchreg;
} IOCONREGS;

/***********************************************************************
 * Memory Multiplexing IOCON Register Bit Field constants
 **********************************************************************/
#define MEMMUX_PIOE_NOMUX       _SBF(0,0)
#define MEMMUX_MIDQM32          _SBF(0,1)
#define MEMMUX_MIDQM30          _SBF(0,3)
#define MEMMUX_PIOE4            _SBF(2,0)
#define MEMMUX_MINWE            _SBF(2,1)
#define MEMMUX_PIOE5            _SBF(3,0)
#define MEMMUX_MISDNCS0         _SBF(3,1)
#define MEMMUX_PIOE6            _SBF(4,0)
#define MEMMUX_MISDNCS1         _SBF(4,1)
#define MEMMUX_PIOE7            _SBF(5,0)
#define MEMMUX_MICKE            _SBF(5,1)
#define MEMMUX_PIOF0            _SBF(6,0)
#define MEMMUX_MICLKIO          _SBF(6,1)
#define MEMMUX_PIO_X            _SBF(7,0)
#define MEMMUX_MIDATA_X         _SBF(7,1)
#define MEMMUX_PIOH2            _SBF(8,0)
#define MEMMUX_MICSN3           _SBF(8,1)
#define MEMMUX_PIOH3            _SBF(9,0)
#define MEMMUX_MICSN4           _SBF(9,1)
#define MEMMUX_PIOH4            _SBF(10,0)
#define MEMMUX_MICSN5           _SBF(10,1)
#define MEMMUX_PIOH5            _SBF(11,0)
#define MEMMUX_MICSN6           _SBF(11,1)
#define MEMMUX_PIOH6            _SBF(12,0)
#define MEMMUX_MIBLSN2          _SBF(12,1)
#define MEMMUX_PIOH7            _SBF(13,0)
#define MEMMUX_MIBLSN3          _SBF(13,1)

/***********************************************************************
 * LCD Multiplexing IOCON Register Bit Field constants
 **********************************************************************/
#define LCDMUX_PIOB4            _SBF(0,0)
#define LCDMUX_CLD12            _SBF(0,1)
#define LCDMUX_CLREV            _SBF(0,2)
#define LCDMUX_PIOB5            _SBF(2,0)
#define LCDMUX_CLD13            _SBF(2,1)
#define LCDMUX_PIOB6            _SBF(3,0)
#define LCDMUX_CLD14            _SBF(3,1)
#define LCDMUX_PIOB7            _SBF(4,0)
#define LCDMUX_CLD15            _SBF(4,1)
#define LCDMUX_CLDSPLEN         _SBF(4,2)
#define LCDMUX_PIOC0            _SBF(6,0)
#define LCDMUX_CLDEN            _SBF(6,1)
#define LCDMUX_CLSPL            _SBF(6,2)
#define LCDMUX_PIOC1            _SBF(8,0)
#define LCDMUX_CLVDDEN          _SBF(8,1)
#define LCDMUX_CLS              _SBF(8,2)
#define LCDMUX_PIOC2            _SBF(10,0)
#define LCDMUX_CLXCLK           _SBF(10,1)
#define LCDMUX_PIOC3            _SBF(11,0)
#define LCDMUX_CLCP             _SBF(11,1)
#define LCDMUX_PIOC4            _SBF(12,0)
#define LCDMUX_CLD16            _SBF(12,1)
#define LCDMUX_PIOC5            _SBF(13,0)
#define LCDMUX_CLLP             _SBF(13,1)
#define LCDMUX_CLP              _SBF(13,2)
#define LCDMUX_PIOC6            _SBF(15,0)
#define LCDMUX_CLD17            _SBF(15,1)
#define LCDMUX_PIOC7            _SBF(16,0)
#define LCDMUX_CLFP             _SBF(16,1)
#define LCDMUX_CLSPS            _SBF(16,2)
#define LCDMUX_PIOD0            _SBF(18,0)
#define LCDMUX_CLD2             _SBF(18,1)
#define LCDMUX_PIOD1            _SBF(19,0)
#define LCDMUX_CLD3             _SBF(19,1)
#define LCDMUX_PIOD2            _SBF(20,0)
#define LCDMUX_CLD4             _SBF(20,1)
#define LCDMUX_PIOD3            _SBF(21,0)
#define LCDMUX_CLD5             _SBF(21,1)
#define LCDMUX_PIOD4            _SBF(22,0)
#define LCDMUX_CLD6             _SBF(22,1)
#define LCDMUX_CPS              _SBF(22,2)
#define LCDMUX_PIOD5            _SBF(24,0)
#define LCDMUX_CLD7             _SBF(24,1)
#define LCDMUX_PIOD6            _SBF(25,0)
#define LCDMUX_CLD8             _SBF(25,1)
#define LCDMUX_PIOD7            _SBF(26,0)
#define LCDMUX_CLD9             _SBF(26,1)
#define LCDMUX_RCEII6           _SBF(27,0)
#define LCDMUX_CLD10            _SBF(27,1)
#define LCDMUX_RCEII7           _SBF(28,0)
#define LCDMUX_CLD11            _SBF(28,1)

/***********************************************************************
 * Miscellaneous Multiplexing IOCON Register Bit Field constants
 **********************************************************************/
#define MISCMUX_PWM1            _SBF(0,0)
#define MISCMUX_DCDEOT1         _SBF(0,1)
#define MISCMUX_PIOA5           _SBF(1,0)
#define MISCMUX_RCCLKOUT        _SBF(1,1)
#define MISCMUX_PIOA6           _SBF(2,0)
#define MISCMUX_RCEII0          _SBF(2,1)
#define MISCMUX_PIOA7           _SBF(3,0)
#define MISCMUX_RCEII1          _SBF(3,1)
#define MISCMUX_PIOB0           _SBF(4,0)
#define MISCMUX_RCEII2          _SBF(4,1)
#define MISCMUX_RCEII3          _SBF(5,0)
#define MISCMUX_PWM0SYNC        _SBF(5,1)
#define MISCMUX_RCEII4          _SBF(6,0)
#define MISCMUX_PWM0            _SBF(6,1)
#define MISCMUX_RCCTOUT         _SBF(7,0)
#define MISCMUX_DCDACK1         _SBF(7,1)
#define MISCMUX_DCDREQ1         _SBF(8,0)
#define MISCMUX_RCEII5          _SBF(8,1)
#define MISCMUX_PIOF1           _SBF(9,0)
#define MISCMUX_RCCLKEN         _SBF(9,1)
#define MISCMUX_RCCLKIN         _SBF(10,0)
#define MISCMUX_RCUTCLK         _SBF(10,1)

/***********************************************************************
 * DMA Multiplexing IOCON Register Bit Field constants
 **********************************************************************/
#define DMAMUX_PIOB1            _SBF(0,0)
#define DMAMUX_DCDEOT0          _SBF(0,1)
#define DMAMUX_PIOB2            _SBF(1,0)
#define DMAMUX_DCDACK0N         _SBF(1,1)
#define DMAMUX_PIOB3            _SBF(2,0)
#define DMAMUX_DCDREQ0          _SBF(2,1)

/***********************************************************************
 * UART Multiplexing IOCON Register Bit Field constants
 **********************************************************************/
#define UARTMUX_U0IRRXA         _SBF(0,0)
#define UARTMUX_U0RXD           _SBF(0,1)
#define UARTMUX_U0IRTXA         _SBF(1,0)
#define UARTMUX_U0TXD           _SBF(1,1)
#define UARTMUX_PA3             _SBF(2,0)
#define UARTMUX_U1RXD           _SBF(2,1)
#define UARTMUX_PA4             _SBF(3,0)
#define UARTMUX_U1TXD           _SBF(3,1)

/***********************************************************************
 * SSI Multiplexing IOCON Register Bit Field constants
 **********************************************************************/
#define SSIMUX_SSPIN            _SBF(0,0)
#define SSIMUX_UT2RXD           _SBF(0,1)
#define SSIMUX_SSPOUT           _SBF(1,0)
#define SSIMUX_UT2TXD           _SBF(1,1)
#define SSIMUX_PIOA0            _SBF(2,0)
#define SSIMUX_SSPENB           _SBF(2,1)
#define SSIMUX_PIOA1            _SBF(3,0)
#define SSIMUX_SSPCLK           _SBF(3,1)
#define SSIMUX_PIOA2            _SBF(4,0)
#define SSIMUX_SSPFRM           _SBF(4,1)

#endif /* LH79520_IOCON_H */ 
