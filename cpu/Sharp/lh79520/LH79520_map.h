/**********************************************************************
 *  $Workfile:   LH79520_map.h  $
 *  $Revision:   1.23  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:24  $
 *
 *  Project: LH79520
 *
 *  Description:
 *      This file contains the Map of the LH79520 System on Chip.
 *      This file depends on the include files which describe the
 *      specific modules on the LH79520.
 *
 *      This file is intended to be included by a target or board
 *      specific include file.  Use the generic names in this file to
 *      define target specific constants in the target specific include
 *      file(s), or in the source files.
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0, Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/Processors/LH79520/LH79520_map.h-arc  $
 * 
 *    Rev 1.23   01 Apr 2002 09:32:24   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.22   Feb 12 2002 14:09:34   BarnettH
 * Changed constant definitions regarding TIMER bases and REG pointers. (Format change only; no substantive change.)
 * 
 *    Rev 1.21   Feb 11 2002 18:09:46   BarnettH
 * Moved xPSR bits to ARM_arch.h and changed names to ARM_x
 * 
 *    Rev 1.20   Feb 01 2002 14:42:42   BarnettH
 * Added default define of REMAP
 * 
 *    Rev 1.19   Feb 01 2002 11:44:56   KovitzP
 * Added comments to explain the REMAP preprocessor define.
 * 
 *    Rev 1.18   Jan 31 2002 17:22:00   SuryanG
 * Included ARM_arch.h
 * 
 *    Rev 1.17   Jan 04 2002 10:32:02   BarnettH
 * Added #error if REMAP is not defined.
 * Added manifest constants for VIC.
 * 
 *    Rev 1.16   Jan 04 2002 10:15:48   BarnettH
 * Changed deprecated #ifndef
 * 
 *    Rev 1.15   Dec 05 2001 09:57:20   BarnettH
 * Deleted unusable predefine
 * Moved _BIT and _SBF definitions to SMA_types.h
 * Changed all Module include files to LH79520_*.h
 * Revised UART definitions
 * Revised IRDA definitions
 * 
 *    Rev 1.14   Dec 04 2001 16:22:34   BarnettH
 * Deleted unusable pre-define
 * Changed reference
 * Changed UART header to LH79520_uart.h
 * 
 *    Rev 1.13   Aug 30 2001 15:07:26   BarnettH
 * Added #include "SMA_types.h"
 * 
 *    Rev 1.12   Aug 28 2001 17:49:38   BarnettH
 * Deleted reference to xxx_lcdc.h: no longer applicable.
 * 
 *    Rev 1.11   Aug 22 2001 15:56:38   BarnettH
 * Added VIC_VECT_MAX
 * 
 *    Rev 1.10   Aug 21 2001 19:28:24   BarnettH
 * Added xSPR bit definitions for IRQ and FIQ bits
 * 
 *    Rev 1.9   Aug 09 2001 18:26:10   BarnettH
 * Added several new VIC #defines.
 * Changed the names of TIMERSx to TIMER0, TIMER1, etc,
 * to be consistent with usage elsewhere.
 * 
 *    Rev 1.8   Aug 03 2001 14:08:30   BarnettH
 * Added include file for MMU
 * Removed local constants defining MMU
 * 
 *    Rev 1.7   Aug 03 2001 13:57:32   BarnettH
 * Safety checkin before moving MMU constants to MMU include file
 * 
 *    Rev 1.6   Jul 19 2001 15:48:56   BarnettH
 * Added SDRAM Controller Memory Bank Address Bases
 * Changed names of MMU_REG c7 and c8
 * Added MMU table Level 1 and Level 2 descriptor field macros
 * 
 *    Rev 1.5   May 22 2001 18:30:50   BarnettH
 * Added MMU and Cache manifest constants (coprocessor 15) for use
 * with inlined assembler called from C code
 * 
 *    Rev 1.4   Apr 23 2001 17:31:02   BarnettH
 * Changed the names of the GPIO typedefs to agree with include file.
 * 
 *    Rev 1.3   Apr 10 2001 08:57:50   BarnettH
 * Completed declaration of IP modules for LH79520.
 * 
 *    Rev 1.1   Apr 04 2001 16:25:22   BarnettH
 * Added several updates for various modules
 * 
 *    Rev 1.0   Apr 03 2001 18:10:48   BarnettH
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

#if !defined LH79520_MAP_H
#define LH79520_MAP_H

#include "SMA_types.h"
#include "ARM_arch.h"

#include "LH79520_clcdc.h"
#include "LH79520_dmac.h"
#include "LH79520_gpio.h"
#include "LH79520_iocon.h"
#include "LH79520_lcdicp.h"
#include "LH79520_mmu.h"
#include "LH79520_pwm.h"
#include "LH79520_rcpc.h"
#include "LH79520_rtc.h"
#include "LH79520_sdrc.h"
#include "LH79520_smc.h"
#include "LH79520_ssp.h"
#include "LH79520_timer.h"
#include "LH79520_uart.h"
#include "LH79520_vic.h"
#include "LH79520_wdt.h"

/**********************************************************************
 * AHB BASES
 *********************************************************************/
#define AHB_BASE            (0xFFFF0000)
#define VIC_BASE_MIRROR     (AHB_BASE + 0x0000)
#define SMC_REGS_BASE       (AHB_BASE + 0x1000)
#define SDRAM_REGS_BASE     (AHB_BASE + 0x2000)
#define LCD_BASE            (AHB_BASE + 0x4000)
#define VIC_BASE            (AHB_BASE + 0xF000)

/**********************************************************************
 * APB BASES
 *********************************************************************/
#define APB_BASE        (0xFFFC0000)
#define UART0_BASE      (APB_BASE + 0x00000)
#define UART1_BASE      (APB_BASE + 0x01000)
#define UART2_BASE      (APB_BASE + 0x02000)
#define PWM_BASE        (APB_BASE + 0x03000)
#define TIMER0_BASE     (APB_BASE + 0x04000)
#define TIMER1_BASE     (APB_BASE + 0x04020)
#define TIMER2_BASE     (APB_BASE + 0x05000)
#define TIMER3_BASE     (APB_BASE + 0x05020)
#define SSP_BASE        (APB_BASE + 0x06000)
#define GPIO3_BASE      (APB_BASE + 0x1C000)
#define GPIO2_BASE      (APB_BASE + 0x1D000)
#define GPIO1_BASE      (APB_BASE + 0x1E000)
#define GPIO0_BASE      (APB_BASE + 0x1F000)
#define RTC_BASE        (APB_BASE + 0x20000)
#define DMAC_BASE       (APB_BASE + 0x21000)
#define RCPC_BASE       (APB_BASE + 0x22000)
#define WDTIMER_BASE    (APB_BASE + 0x23000)
#define LCDICP_BASE     (APB_BASE + 0x24000)
#define IOCON_BASE      (APB_BASE + 0x25000)

#define SDRAM_MEM_BASE              (0x20000000)
#define SMC_MEM_BASE                (0x40000000)
#define INTERNAL_MEM_BASE           (0x60000000)

/**********************************************************************
 * REMAP  (See the Memory Map section of the ARM Core and Buses
 *         chapter of the LH79520 User's Guide. The LH79520 Memory
 *         Map Variations figure shows how REMAP value determines
 *         whether the SMC, SDRAM, or internal SRAM is mirrored to
 *         the first 512 Mbytes of the LH79520's physical memory map)
 *
 *         Use the preprocessor define mechanism for your build
 *         environment to define the REMAP value.
 *********************************************************************/
#if !defined REMAP
#define REMAP 0
#endif

#if REMAP == 0 
#define SMC_MIRROR_MEM_BASE         (0x00000000)
#elif REMAP == 1 
#define SDRAM_MIRROR_MEM_BASE       (0x00000000)
#elif REMAP == 2 
#define INTERNAL_MIRROR_MEM_BASE    (0x00000000)
#else
#error REMAP must be defined as 0, 1, or 2
#endif

/**********************************************************************
 * SMC Memory Bank Address Space Bases
 *********************************************************************/

#define SMC_BANK0_BASE      (SMC_MEM_BASE + 0x00000000)
#define SMC_BANK1_BASE      (SMC_MEM_BASE + 0x04000000)
#define SMC_BANK2_BASE      (SMC_MEM_BASE + 0x08000000)
#define SMC_BANK3_BASE      (SMC_MEM_BASE + 0x0C000000)
#define SMC_BANK4_BASE      (SMC_MEM_BASE + 0x10000000)
#define SMC_BANK5_BASE      (SMC_MEM_BASE + 0x14000000)
#define SMC_BANK6_BASE      (SMC_MEM_BASE + 0x18000000)
#define SMC_BANK7_BASE      (SMC_MEM_BASE + 0x1C000000)

/**********************************************************************
 * SDRAMC Memory Bank Address Space Bases
 *********************************************************************/

#define SDRAM_BANK0_BASE        (SDRAM_MEM_BASE + 0x00000000)
#define SDRAM_BANK1_BASE        (SDRAM_MEM_BASE + 0x08000000)

/**********************************************************************
 * Vectored Interrupt Controller (VIC)
 *********************************************************************/
#define VIC         ((VICREGS *)(VIC_BASE))
#define VICMIRROR   ((VICREGS *)(VIC_BASE_MIRROR))

/**********************************************************************
 * Static Memory Controller (SMC)
 *********************************************************************/
#define SMC     ((SMCREGS *)(SMC_REGS_BASE))

/**********************************************************************
 * SDRAM Controller (SDRAM)
 *********************************************************************/
#define SDRAM   ((SDRAMREGS *)(SDRAM_REGS_BASE))

/**********************************************************************
 * Color LCD Controller (CLCDC)
 *********************************************************************/
#define CLCDC   ((CLCDCREGS *)(LCD_BASE))

/**********************************************************************
 * UART'S
 *********************************************************************/
#define UART0   ((UARTREGS *)(UART0_BASE))
#define UART1   ((UARTREGS *)(UART1_BASE))
#define UART2   ((UARTREGS *)(UART2_BASE))

/**********************************************************************
 * IRDA
 *********************************************************************/
#define IRDA    ((UARTREGS *)(UART0_BASE))

/**********************************************************************
 * Pulse Width Modulator (PWM)
 *********************************************************************/
#define PWMX_OFFSET (0x20)
#define PWM     ((PWMREGS *)(PWM_BASE))
#define PWM0    ((PWMXREGS *)(PWM_BASE))
#define PWM1    ((PWMXREGS *)(PWM_BASE + PWMX_OFFSET))

/**********************************************************************
 * TIMER
 *********************************************************************/
#define TIMER0  ((TIMERREG *)(TIMER0_BASE))
#define TIMER1  ((TIMERREG *)(TIMER1_BASE))
#define TIMER2  ((TIMERREG *)(TIMER2_BASE))
#define TIMER3  ((TIMERREG *)(TIMER3_BASE))

/**********************************************************************
 * Synchronous Serial Port (SSP)
 *********************************************************************/
#define SSP     ((SSPREGS *)(SSP_BASE))

/**********************************************************************
 * General Purpose Input/Output (GPIO)
 *********************************************************************/
#define GPIOA   ((GPIOPAREGS *)(GPIO0_BASE))
#define GPIOB   ((GPIOPBREGS *)(GPIO0_BASE))
#define GPIOC   ((GPIOPAREGS *)(GPIO1_BASE))
#define GPIOD   ((GPIOPBREGS *)(GPIO1_BASE))
#define GPIOE   ((GPIOPAREGS *)(GPIO2_BASE))
#define GPIOF   ((GPIOPBREGS *)(GPIO2_BASE))
#define GPIOG   ((GPIOPAREGS *)(GPIO3_BASE))
#define GPIOH   ((GPIOPBREGS *)(GPIO3_BASE))

/**********************************************************************
 * Real Time Clock (RTC)
 *********************************************************************/
#define RTC     ((RTCREGS *)(RTC_BASE))

/**********************************************************************
 * DMA Controller (DMAC)
 *********************************************************************/
#define DMAC    ((DMACREGS *)(DMAC_BASE))

/**********************************************************************
 * Reset, Clock, and Power Controller (RCPC)
 *********************************************************************/
#define RCPC    ((RCPCREGS *)(RCPC_BASE))   

/**********************************************************************
 * Watchdog Timer (WDTIMER)
 *********************************************************************/
#define WDTIMER     ((WDTIMERREGS *)(WDTIMER_BASE))

/**********************************************************************
 * LCD Interface Control Processor (LCDICP)
 *********************************************************************/
#define LCDICP      ((LCDICPREGS *)(LCDICP_BASE))

/**********************************************************************
 * IOCON
 *********************************************************************/
#define IOCON   ((IOCONREGS *)(IOCON_BASE))

#endif /* LH79520_MAP_H */ 
