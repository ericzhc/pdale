/**********************************************************************
 * $Workfile:   LH79520_clcdc.h  $
 * $Revision:   1.3  $
 * $Author:   kovitzp  $
 * $Date:   01 Apr 2002 09:32:22  $
 *
 * Project: LH79520 headers
 *
 * Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          Color LCD Controller
 *
 *  Note (1): Several LCD macros calculate a value by subtracting
 *      1 or 2 from an argument.  The programmer is responsible for
 *      ensuring that the argument is >= 1 or >= 2, as applicable.
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 * Revision History:
 * $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_clcdc.h-arc  $
 * 
 *    Rev 1.3   01 Apr 2002 09:32:22   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.2   Jan 22 2002 09:40:50   SuryanG
 * Corrected value of constants CLCDC_LCDCTRL_VCOMP_AV 
 * and CLCDC_LCDCTRL_VCOMP_FP.
 * 
 *    Rev 1.1   Jan 04 2002 10:06:36   BarnettH
 * Changed deprecated #ifndef 
 * 
 *    Rev 1.0   Dec 05 2001 09:38:46   BarnettH
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
 * COPYRIGHT (C) 2001 SHARP MICROELECTRONICS OF THE AMERICAS, INC.
 *             CAMAS, WA
 *********************************************************************/
#if !defined LH79520_CLCDC_H
#define LH79520_CLCDC_H

#include "SMA_types.h"

/* LCD Controller  STN and TFT Panel Format Registers */ 
typedef struct 
{
    volatile UNS_32 lcdtiming0;
    volatile UNS_32 lcdtiming1;
    volatile UNS_32 lcdtiming2;
    volatile UNS_32 lcdtiming3;
    volatile UNS_32 lcdupbase;
    volatile UNS_32 lcdlpbase;
    volatile UNS_32 lcdintrenable;
    volatile UNS_32 lcdctrl;
    volatile UNS_32 lcdstatus;
    volatile UNS_32 lcdinterrupt;
    volatile UNS_32 lcdupcurr;
    volatile UNS_32 lcdlpcurr;
    volatile UNS_32 reservedclcdc[116];
    volatile UNS_32 lcdpalette[128];
} CLCDCREGS;

/**********************************************************************
 * Color LCD Controller LCDTiming0 Register Bit Field constants
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than zero.
 *********************************************************************/ 
/* LCDTiming0 register field position LSBs */
/* Pixels per line */ 
/* Calculates PPL field value from actual pixels per line */ 
#define CLCDC_LCDTIMING0_PPL_WIDTH 6
#define CLCDC_LCDTIMING0_PPL(n) \
        _SBF(2,(((n)/16)-1)&_BITMASK(CLCDC_LCDTIMING0_PPL_WIDTH))
/* Horizontal Synchronization Pulse Width */ 
/* Calculates HSW field value from width of CLLP signal in
 * CLCP periods */ 
#define CLCDC_LCDTIMING0_HSW_WIDTH 8
#define CLCDC_LCDTIMING0_HSW(n) \
        _SBF(8,((n)-1)&_BITMASK(CLCDC_LCDTIMING0_HSW_WIDTH))
/* Horizontal Front Porch */ 
/* Calculates HFP field value from CLCP periods */
#define CLCDC_LCDTIMING0_HFP_WIDTH 8
#define CLCDC_LCDTIMING0_HFP(n) \
        _SBF(16,(((n)-1)&_BITMASK(CLCDC_LCDTIMING0_HFP_WIDTH)))
/* Horizontal Back Porch */ 
/* Calculates HBP field value from CLCP periods */  
#define CLCDC_LCDTIMING0_HBP_WIDTH 8
#define CLCDC_LCDTIMING0_HBP(n) \
        _SBF(24,(((n)-1)&_BITMASK(CLCDC_LCDTIMING0_HBP_WIDTH)))

/**********************************************************************
 * Color LCD Controller LCDTiming1 Register Bit Field constants
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than zero.
 *********************************************************************/ 
/* Lines per Panel */ 
/* Calculates LPP field value from actual lines per panel */ 
#define CLCDC_LCDTIMING1_LPP_WIDTH 10
#define CLCDC_LCDTIMING1_LPP(n) \
        _SBF(0,((n)-1)&_BITMASK(CLCDC_LCDTIMING1_LPP_WIDTH))
/* Vertical Synchronization Pulse */ 
/* Calculates VSW field value from vertical sync lines */ 
#define CLCDC_LCDTIMING1_VSW_WIDTH 6
#define CLCDC_LCDTIMING1_VSW(n) \
        _SBF(10,((n)-1)&_BITMASK(CLCDC_LCDTIMING1_VSW_WIDTH))
/* Vertical Front Porch */ 
/* Calculates VFP field value from inactive lines at end of frame */ 
#define CLCDC_LCDTIMING1_VFP_WIDTH 8 
#define CLCDC_LCDTIMING1_VFP(n) \
        _SBF(16,((n)-1)&_BITMASK(CLCDC_LCDTIMING1_VFP_WIDTH))
/* Vertical Back Porch */ 
/* Calculates VBP field value from inactive lines at start of frame */ 
#define CLCDC_LCDTIMING1_VBP_WIDTH 8
#define CLCDC_LCDTIMING1_VBP(n) \
        _SBF(24,((n)-1)&_BITMASK(CLCDC_LCDTIMING1_VBP_WIDTH))

/**********************************************************************
 * Color LCD Controller LCDTiming2 Register Bit Field constants
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than two.
 *********************************************************************/ 
/* Panel Clock Divisor */ 
#define CLCDC_LCDTIMING2_PCD_MIN 2
#define CLCDC_LCDTIMING2_PCD_WIDTH 5
#define CLCDC_LCDTIMING2_PCD(n) \
        _SBF(0,((n)-2)&_BITMASK(CLCDC_LCDTIMING2_PCD_WIDTH))
/* Clock Selector */ 
#define CLCDC_LCDTIMING2_CLKSEL _BIT(5)
/* AC Bias Pin Frequency */ 
#define CLCDC_LCDTIMING2_ACB_WIDTH 5
#define CLCDC_LCDTIMING2_ACB(n) \
        _SBF(6,((n)-1)&_BITMASK(CLCDC_LCDTIMING2_ACB_WIDTH))
/* Invert Vertical Synchronization */ 
#define CLCDC_LCDTIMING2_IVS    _BIT(11)
/* Invert Horizontal Synchronization */ 
#define CLCDC_LCDTIMING2_IHS    _BIT(12)
/* Invert Panel Clock */ 
#define CLCDC_LCDTIMING2_IPC    _BIT(13)
/* Invert Output Enable */ 
#define CLCDC_LCDTIMING2_IOE    _BIT(14)
/* Clocks Per Line */ 
/* The CPL macro does NOT calculate a bit field value; the programmer
 * must establish the calling argument */ 
#define CLCDC_LCDTIMING2_CPL_WIDTH 10
#define CLCDC_LCDTIMING2_CPL(n) \
        _SBF(16,(n)&_BITMASK(CLCDC_LCDTIMING2_CPL_WIDTH))
/* Bypass Pixel Clock Divider */
#define CLCDC_LCDTIMING2_BCD    _BIT(26)

/**********************************************************************
 * Color LCD Controller LCDTiming3 Register Bit Field constants
 *
 * NOTE: Ensure the argument to the following macros is greater
 * than one.
 *********************************************************************/ 
/* Line End Signal Delay */ 
/* Calculates LED field value from number of CLCDCLK periods */ 
#define CLCDC_LCDTIMING3_LED_WIDTH 7
#define CLCDC_LCDTIMING3_LED(n) \
        _SBF(0,((n)-1)&_BITMASK(CLCDC_LCDTIMING3_LED_WIDTH))
/* Line End Enable */ 
#define CLCDC_LCDTIMING3_LEE    _BIT(16)

/**********************************************************************
 * Color LCD Controller 
 *  LCD Interrupt Enable Register
 *  LCD Status Register
 *  LCD Interrupt Register
 * Bit Field constants
 *********************************************************************/ 
/* LCDIntrEnable, LCDInterrupt, LCDStatus bit field positions */
#define CLCDC_LCDSTATUS_FUF     _BIT(1) /* FIFO underflow */ 
#define CLCDC_LCDSTATUS_LNBU    _BIT(2) /* LCD Next addr. Base Update*/ 
#define CLCDC_LCDSTATUS_VCOMP   _BIT(3) /* Vertical Compare */ 
#define CLCDC_LCDSTATUS_MBERROR _BIT(4) /* Master Bus Error */ 

/**********************************************************************
 * Color LCD Controller Control Register Bit Field constants
 *********************************************************************/ 
/* register field position LSBs LCDCtrl */
/* LCD Controller Enable */ 
#define CLCDC_LCDCTRL_ENABLE    _BIT(0)
/* Bits per pixel */ 
#define CLCDC_LCDCTRL_BPP1      _SBF(1,0)
#define CLCDC_LCDCTRL_BPP2      _SBF(1,1)
#define CLCDC_LCDCTRL_BPP4      _SBF(1,2)
#define CLCDC_LCDCTRL_BPP8      _SBF(1,3)
#define CLCDC_LCDCTRL_BPP16     _SBF(1,4)
#define CLCDC_LCDCTRL_BPP24     _SBF(1,5)
/* STN LCD Mono or Color */ 
#define CLCDC_LCDCTRL_BW_COLOR  _SBF(4,0)
#define CLCDC_LCDCTRL_BW_MONO   _SBF(4,1)
/* TFT LCD */ 
#define CLCDC_LCDCTRL_TFT       _BIT(5)
/* Monochrome LCD has 8-bit interface */ 
#define CLCDC_LCDCTRL_MON8      _BIT(6)
/* Dual Panel STN */ 
#define CLCDC_LCDCTRL_DUAL      _BIT(7)
/* Swap Red and Blue (RGB to BGR) */ 
#define CLCDC_LCDCTRL_BGR       _BIT(8)
/* Big Endian Byte Order */ 
#define CLCDC_LCDCTRL_BEBO      _BIT(9)
/* Big Endian Pixel Order */ 
#define CLCDC_LCDCTRL_BEPO      _BIT(10)
/* LCD Power Enable */ 
#define CLCDC_LCDCTRL_PWR       _BIT(11)
/* VCOMP Interrupt
 * 2 bits, generate Interrupt at:
 * 00 - start of vertical synch
 * 01 - start of back porch
 * 10 - start of active video
 * 11 - start of front porch
 */ 
#define CLCDC_LCDCTRL_VCOMP_VS 0
#define CLCDC_LCDCTRL_VCOMP_BP 1
#define CLCDC_LCDCTRL_VCOMP_AV 2
#define CLCDC_LCDCTRL_VCOMP_FP 3
#define CLCDC_LCDCTRL_VCOMP(n) \
        _SBF(12,(n))
/* LCD DMA FIFO Test Mode Enable */ 
#define CLCDC_LCDCTRL_LDMAFIFOTME   _BIT(15)
/* LCD DMA FIFO Watermark Level */ 
#define CLCDC_LCDCTRL_WATERMARK _BIT(16)

#endif /*  LH79520_CLCDC_H */ 
