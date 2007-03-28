/**********************************************************************
 *  $Workfile:   LH79520_rcpc.h  $
 *  $Revision:   1.2  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:50  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for LH79520 SOC:
 *
 *          Reset, Clock, and Power Controller
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_rcpc.h-arc  $
 * 
 *    Rev 1.2   01 Apr 2002 09:32:50   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.1   Jan 04 2002 10:41:34   BarnettH
 * Changed deprecated #ifndef.
 * Changed structure member names to lower-case.
 * Changed "Spare0Prescale" to lcdclkprescale.
 * Changed "Spare1Prescale" to sspclkprescale
 * Changed "Spare[2:14]Prescale" registers to "reserved".
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

#if !defined LH79520_RCPC_H
#define LH79520_RCPC_H

#include "SMA_types.h"

/**********************************************************************
 * Reset, Clock, and Power Controller Register Structure
 *********************************************************************/ 
typedef struct 
{
    volatile UNS_32 rcpcctrl;        /* RCPC Control */ 
    volatile UNS_32 reserved1;       /* not used */ 
    volatile UNS_32 remap;           /* Remap Control */ 
    volatile UNS_32 softreset;       /* Soft Reset */ 
    volatile UNS_32 resetstatus;     /* Reset Status */ 
    volatile UNS_32 resetstatusclr;  /* Reset Status Clear */ 
    volatile UNS_32 hclkprescale;    /* HCLK Prescale */ 
    volatile UNS_32 cpuclkprescale;  /* Core Clock Prescale */ 
    volatile UNS_32 pclkprescale;    /* PCLK Prescale */ 
    volatile UNS_32 periphclkctrl;   /* Peripheral Clock Ctrl */ 
    volatile UNS_32 spareclkctrl;    /* Spare Clock Ctrl */ 
    volatile UNS_32 ahbclkctrl;      /* AHB Clock Ctrl */ 
    volatile UNS_32 periphclksel;    /* Peripheral Clock Select*/
    volatile UNS_32 spareclksel;     /* Spare Clock Select */
    volatile UNS_32 pwm0prescale;    /* PWM 0 Prescale*/
    volatile UNS_32 pwm1prescale;    /* PWM 1 Prescale*/
    volatile UNS_32 lcdclkprescale;  /* LCD clock Prescale */
    volatile UNS_32 sspclkprescale;  /* SSP clock Prescale */
    volatile UNS_32 reserved2[14];   /* not used */ 
    volatile UNS_32 intconfig;       /* Ext. Interrupt Config */
    volatile UNS_32 intclear;        /* Ext. Interrupt Clear  */
    volatile UNS_32 coreclkconfig;   /* Core Clock Config */
} RCPCREGS;

/**********************************************************************
 * Reset, Clock, and Power Controller Register Bit Fields
 *********************************************************************/ 

/**********************************************************************
 * rcpcctrl - RCPCCtrl Register Bit Fields
 *********************************************************************/ 
#define RCPC_CTRL_EP        _BIT(0) /* Enable PLL */ 
#define RCPC_CTRL_EX        _BIT(1) /* Enable Internal XTAL */ 

#define RCPC_CTRL_PWRDWNSEL(n)  _SBF(2,(n)) /* Power Down Mode Sel*/ 
#define RCPC_PWRDWNSEL_ACTIVE    RCPC_CTRL_PWRDWNSEL(0)
#define RCPC_PWRDWNSEL_STANDBY   RCPC_CTRL_PWRDWNSEL(1)
#define RCPC_PWRDWNSEL_SLEEP     RCPC_CTRL_PWRDWNSEL(2)
#define RCPC_PWRDWNSEL_STOP1     RCPC_CTRL_PWRDWNSEL(3)
#define RCPC_PWRDWNSEL_STOP2     RCPC_CTRL_PWRDWNSEL(4)

#define RCPC_CTRL_OUTSEL(n)     _SBF(5,(n))
#define RCPC_OUTSEL_CLK_INTOSC   RCPC_CTRL_OUTSEL(0)
#define RCPC_OUTSEL_CLK_PLL      RCPC_CTRL_OUTSEL(1)
#define RCPC_OUTSEL_FCLK_CPU     RCPC_CTRL_OUTSEL(2)
#define RCPC_OUTSEL_HCLK         RCPC_CTRL_OUTSEL(3)

#define RCPC_CTRL_CLKSEL_PLL    _SBF(7,0)
#define RCPC_CTRL_CLKSEL_EXT    _SBF(7,1)

#define RCPC_CTRL_WRTLOCK_LOCKED    _SBF(9,0)
#define RCPC_CTRL_WRTLOCK_ENABLED   _SBF(9,1)

/**********************************************************************
 * remap - Remap Control Register Bit Fields
 *********************************************************************/ 
#define RCPC_REMAP_SMEM0    (0)
#define RCPC_REMAP_SDMEM0   (1)
#define RCPC_REMAP_IMEM0    (2)

/**********************************************************************
 * softreset - Soft Reset Register Bit Fields
 *********************************************************************/ 
#define RCPC_SOFTRESET_ALL  (0xDEAD)
#define RCPC_SOFTRESET_GBL  (0xDEAC)

/**********************************************************************
 * resetstatus, resetstatusclr - Reset Status Register Bit Fields
 *********************************************************************/ 
#define RCPC_RESET_STATUS_EXT   _BIT(0)
#define RCPC_RESET_STATUS_WDTO  _BIT(1)

/**********************************************************************
 * hclkPrescale - HCLK Prescaler Register Bit Fields
 * cpuclkPrescale - Core Clock Prescaler Register Bit Fields
 * pclkPrescale - PCLK Prescaler Register Bit Fields
 * pwm0Prescale - PWM0 Prescaler Register Bit Fields
 * pwm1Prescale - PWM1 Prescaler Register Bit Fields
 * sparePrescale - Spare Prescaler Register Bit Fields
 * Note: not all constants are applicable to all registers.
 *  See Reference.
 *********************************************************************/ 
#define RCPC_PRESCALER_DIV1     _SBF(0,0)
#define RCPC_PRESCALER_DIV2     _SBF(0,1)
#define RCPC_PRESCALER_DIV4     _SBF(0,2)
#define RCPC_PRESCALER_DIV6     _SBF(0,3)
#define RCPC_PRESCALER_DIV8     _SBF(0,4)
#define RCPC_PRESCALER_DIV16    _SBF(0,8)
#define RCPC_PRESCALER_DIV30    _SBF(0,15)
#define RCPC_PRESCALER_DIV32    _SBF(0,16)
#define RCPC_PRESCALER_DIV64    _SBF(0,32)
#define RCPC_PRESCALER_DIV128   _SBF(0,64)
#define RCPC_PRESCALER_DIV256   _SBF(0,128)
#define RCPC_PRESCALER_DIV65534 (0xFFFF)

/**********************************************************************
 * periphclkctrl - Peripheral Clock Control Register Bit Fields
 * spareclkctrl - Spare Clock Control Register Bit Fields
 * ahbclkctrl - AHB Clock Control Register Bit Fields
 * Writing a "0" to a bit in these registers enables the
 *  corresponding clock
 *********************************************************************/ 
#define RCPC_CLKCTRL_UO_DISABLE         _BIT(0)
#define RCPC_CLKCTRL_U1_DISABLE         _BIT(1)
#define RCPC_CLKCTRL_U2_DISABLE         _BIT(2)
#define RCPC_CLKCTRL_CT0_DISABLE        _BIT(3)
#define RCPC_CLKCTRL_CT1_DISABLE        _BIT(4)
#define RCPC_CLKCTRL_CT2_DISABLE        _BIT(5)
#define RCPC_CLKCTRL_CT3_DISABLE        _BIT(6)
#define RCPC_CLKCTRL_PWM0_DISABLE       _BIT(7)
#define RCPC_CLKCTRL_PWM1_DISABLE       _BIT(8)
#define RCPC_CLKCTRL_RTC_DISABLE        _BIT(9)
#define RCPC_CLKCTRL_SPARE_DISABLE(f)   _BIT(f)
#define RCPC_CLKCTRL_SSPCLK_DISABLE     _BIT(1)
#define RCPC_CLKCTRL_LCDCLK_DISABLE     _BIT(0)
#define RCPC_CLKCTRL_DMAC_DISABLE       _BIT(0)
#define RCPC_CLKCTRL_HCLKSP0_DISABLE    _BIT(1)

/**********************************************************************
 * periphclksel - Peripheral Clock Select Register Bit Fields
 * Writing a "0" to U0-U2 in this register enables the
 *  XTAL Oscillator as the clock source
 * Writing a "0" to CT0-CT3 in this register enables the
 *  HCLK as the clock source
 *********************************************************************/ 
#define RCPC_PCLKSEL_U0_EXT     _BIT(0) /* U0 Clock Source */ 
#define RCPC_PCLKSEL_U1_EXT     _BIT(1) /* U1 Clock Source */ 
#define RCPC_PCLKSEL_U2_EXT     _BIT(2) /* U2 Clock Source */ 
#define RCPC_PCLKSEL_CT0_EXT    _BIT(3) /* CT0 Clock Source */ 
#define RCPC_PCLKSEL_CT1_EXT    _BIT(4) /* CT1 Clock Source */ 
#define RCPC_PCLKSEL_CT2_EXT    _BIT(5) /* CT2 Clock Source */ 
#define RCPC_PCLKSEL_CT3_EXT    _BIT(6) /* CT3 Clock Source */ 
#define RCPC_PCLKSEL_RTC_32     0       /* RTC Clock Source 32KHz */ 
#define RCPC_PCLKSEL_RTC_EXT    _SBF(7,2) /* RTC Clock Source Ext */

/**********************************************************************
 * spareclksel - Peripheral Clock Select Register Bit Fields
 * Writing a "0" to a bit in this register enables the
 *  HCLK as the clock source
 *********************************************************************/ 
#define RCPC_SCLKSEL_SP(n)      _SBF((n),1) /* SP2 - SP15 */ 
#define RCPC_SCLKSEL_SSPCLK     _BIT(1) /* SSP Clock External */ 
#define RCPC_SCLKSEL_LCDCLK     _BIT(0) /* LCD Clock External */ 

/**********************************************************************
 * intconfig - External Interrupt Configuration Register Bit Fields
 *********************************************************************/ 
#define RCPC_INTCONFIG(f,v)     _SBF((f),(v))
/* RCPC_INTCONFIG arguments for 'f' parameter */ 
#define RCPC_INT0       0
#define RCPC_INT1       2
#define RCPC_INT2       4
#define RCPC_INT3       6
#define RCPC_INT4       8
#define RCPC_INT5       10
#define RCPC_INT6       12
#define RCPC_INT7       14
/* RCPC_INTCONFIG arguments for 'v' parameter */ 
#define RCPC_INT_LLT    0       /* Low Level Trigger */ 
#define RCPC_INT_HLT    1       /* High Level Trigger */ 
#define RCPC_INT_FET    2       /* Falling Edge Trigger */ 
#define RCPC_INT_RET    3       /* Rising Edge Trigger */ 

/**********************************************************************
 * intclear - External Interrupt Clear Register Bit Fields
 *********************************************************************/ 
#define RCPC_INTCLEAR(n)    _BIT(n) /* Clear Edge Interrupt 'n' */ 

/**********************************************************************
 * coreclkconfig - Core Clock Configuration Register Bit Fields
 *********************************************************************/ 
#define RCPC_CCC_STDASYNCH      0 /* Standard Mode, Asynch operation */
#define RCPC_CCC_FASTBUS        1 /* Fast Bus Extension Mode */
#define RCPC_CCC_STDSYNCH       2 /* Standard Mode, Synch operation */

#endif /* LH79520_RCPC_H */ 
