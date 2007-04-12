/**********************************************************************
 *  $Workfile:   LH79520_evb.h  $
 *  $Revision:   1.19  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:59:52  $
 *
 *  Project: LH79520 Evaluation Board
 *
 *  Description:
 *      This file contains the following information for the
 *      LH79520 Evaluation Board:
 *          includes the LH79520 Map file
 *          includes board-specific include files which include:
 *              board-specific primitive include files
 *              board-specific function prototype declarations
 *              board-specific function manifest constants
 *          board-specific typedefs
 *          board-specific manifest constants
 *          sets pertinent board-specific "#ifdef" constants
 *          notes on specific board usage
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0, Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/Platforms/LH79520/LH79520_evb.h-arc  $
 * 
 *    Rev 1.19   01 Apr 2002 09:59:52   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.18   Feb 12 2002 11:11:50   BarnettH
 * Added include file SMA_sevenseg_driver.h
 * 
 *    Rev 1.17   Jan 04 2002 10:13:40   KovitzP
 * CPLD bit descriptions movedfrom various driver files  to 
 * LH79520_evb.h .
 * 
 *    Rev 1.16   Jan 03 2002 18:00:46   BarnettH
 * Added #undef's for LH79520_CLKIN_FREQ
 * Removed comment wrappers from #define's for LH79520_CLKIN_FREQ
 * 
 *    Rev 1.15   Jan 03 2002 17:05:36   BarnettH
 * Moved "ERROR" macros to SMA_types.h
 * Added CPLD helper macros
 * Changed LCD_CLKIN_FREQ's to be set according to pre-define.
 * Changed the name of Miscellaneous Constants
 * Moved EVB Memory (Physical) Constants and BootROM constants to more appropriate locations.
 * 
 *    Rev 1.14   Dec 12 2001 15:14:22   BarnettH
 * Provided "extern" declarations for MMU_LEVEL1_DESCRIPTORS 
 * arguments, cast argument
 * 
 *    Rev 1.13   Dec 12 2001 13:42:04   BarnettH
 * Moved OS_TIMER and OS_VIC_* constants to LH79520_ucosii.h
 * 
 *    Rev 1.12   Dec 12 2001 09:28:42   BarnettH
 * Removed unusable pre-define.
 * Moved _BIT and _SBF to SMA_types.
 * Changed types to SMA_types.
 * Moved user entries back to bottom of file.
 * Changed OS_TIMER to use TIMER0
 * 
 *    Rev 1.11   Oct 24 2001 09:43:38   BarnettH
 * Removed "..._sevenseg.h" include
 * 
 *    Rev 1.10   Sep 05 2001 15:14:56   KovitzP
 * changed unsigned short to UNS_16 per SMA_types.h. Added details about the memory map and initial stacks.
 * 
 *    Rev 1.9   Aug 30 2001 14:42:00   BarnettH
 * Added #include "SMA_types.h"
 *
 *    Rev 1.8   Aug 28 2001 10:23:14   BarnettH
 * Added LCD Clock Definitions
 * 
 *    Rev 1.7   Aug 24 2001 10:48:08   BarnettH
 * Added LED_ON, LED_OFF, and LED_TOGGLE
 * 
 *    Rev 1.6   Aug 22 2001 15:55:22   BarnettH
 * Added some uC/OS-II constants.
 * Added discrete sysclock frequency defines
 * 
 *    Rev 1.5   Aug 21 2001 19:26:52   BarnettH
 * Added uC/OS-II OS_TIMER #defines.
 * 
 *    Rev 1.4   Aug 09 2001 18:34:38   BarnettH
 * Added LED #defines.
 * 
 *    Rev 1.3   Aug 03 2001 14:16:06   BarnettH
 * Added CPLD structure and manifest constants.
 * Added manifest constants to allow selection of alternate MMU Level 1 Descriptors array for MMU initialization.
 * 
 *    Rev 1.2   Jul 27 2001 11:22:48   BarnettH
 * Added some CLKIDX and REFTIMER defines.
 * 
 *    Rev 1.1   Jul 26 2001 14:31:16   BarnettH
 * Added some 'ERROR' #define's
 * Added the IOCON MemMux Init value
 * Added some RCPC #define's
 * 
 *    Rev 1.0   Apr 10 2001 09:31:18   BarnettH
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

#if !defined LH79520_EVB_H
#define LH79520_EVB_H

#include "SMA_types.h"
#include "LH79520_map.h"
#include "SMA_sevenseg_driver.h"

/**********************************************************************
 * CPLD addresses
 *********************************************************************/
#define CPLD_BASE   (SMC_BANK2_BASE)

typedef struct 
{
    volatile UNS_16 keys_status;
    volatile UNS_16 reserved1;
    volatile UNS_16 l3_reg;
    volatile UNS_16 reserved2;
    volatile UNS_16 lcd_pwr_cntl;
    volatile UNS_16 reserved3;
    volatile UNS_16 l3_mode;
    volatile UNS_16 reserved4;
    volatile UNS_16 gpi;
    volatile UNS_16 reserved5;
    volatile UNS_16 gpo;
    volatile UNS_16 reserved6;
    volatile UNS_16 adc_dac_left;
    volatile UNS_16 adc_dac_right;
    volatile UNS_16 audio_control;
    volatile UNS_16 reserved7;
    volatile UNS_16 display_dip_sw;
    volatile UNS_16 reserved8;
    volatile UNS_16 seven_seg;
    volatile UNS_16 reserved9;
    volatile UNS_16 misc_stat;
    volatile UNS_16 reserved10;
    volatile UNS_16 gpio_data_dir;
    volatile UNS_16 reserved11;
    volatile UNS_16 ssp_dev_sel;
    volatile UNS_16 reserved12;
    volatile UNS_16 ser_port1_rts;
    volatile UNS_16 reserved13;
    volatile UNS_16 cf_reset;
    volatile UNS_16 reserved14;
    volatile UNS_16 cpu_dip_sw;
    volatile UNS_16 reserved15;
    volatile UNS_16 intr_mask;
    volatile UNS_16 reserved16;
    volatile UNS_16 reserved17;
    volatile UNS_16 reserved18;
    volatile UNS_16 reserved19;
    volatile UNS_16 reserved20;
    volatile UNS_16 reserved21;
    volatile UNS_16 reserved22;
    volatile UNS_16 nio_reg_clk;
    volatile UNS_16 reserved23;
} CPLDREGS;

#define CPLD    ((CPLDREGS *)(CPLD_BASE))

/***********************************************************************
 * CPLD Register Bits
 **********************************************************************/
/* key_status register bits and pushbutton-related intr_mask bits */
#define CPLD_PB_DOWN    _BIT(0)
#define CPLD_PB_SW7     (CPLD_PB_DOWN)
#define CPLD_PB_RIGHT   _BIT(1)
#define CPLD_PB_SW6     (CPLD_PB_RIGHT)
#define CPLD_PB_LEFT    _BIT(2)
#define CPLD_PB_SW5     (CPLD_PB_LEFT)
#define CPLD_PB_UP      _BIT(3)
#define CPLD_PB_SW4     (CPLD_PB_UP)
#define CPLD_INT        _BIT(4)
#define CPLD_PB_SW2     (CPLD_INT)
#define CPLD_USER_BUTTON_MASK (CPLD_PB_DOWN | CPLD_PB_RIGHT \
                               | CPLD_PB_LEFT | CPLD_PB_UP)

/* LCD power bits */
#define CPLD_EN26V         _BIT(0)
#define CPLD_BACKLIGHT_ON  _BIT(1)
#define CPLD_DISP_EN       _BIT(2)
#define CPLD_LCD_OE        _BIT(3)
#define CPLD_LCDP_EN       _BIT(4)
 
/* L3 mode bits */
#define CPLD_L3_MODE_HI         (CPLD->l3_mode |= _BIT(0))
#define CPLD_L3_MODE_LOW        (CPLD->l3_mode |= ~(_BIT(0)))

/* I2S audio control register bits */
#define CPLD_DAC_USE_REQ1           _BIT(12)
#define CPLD_ADC_DMA_ENABLE         _BIT(7)
#define CPLD_DAC_DMA_ENABLE         _BIT(6)
#define CPLD_ADC_DMA_AUTO           _BIT(5)
#define CPLD_DAC_DMA_AUTO           _BIT(4)
#define CPLD_ADC_IRQ_ENABLE         _BIT(3)
#define CPLD_DAC_IRQ_ENABLE         _BIT(2)
#define CPLD_ADC_IRQ_STATUS         _BIT(1)
#define CPLD_DAC_IRQ_STATUS         _BIT(0)
#define CPLD_AUDIO_DAC_INT_PENDING  _BIT(0)
#define CPLD_AUDIO_ADC_INT_PENDING  _BIT(1)
#define CPLD_AUDIO_DAC_INT_ENALBED  _BIT(2)
#define CPLD_AUDIO_ADC_INT_ENABLED  _BIT(3)
#define CPLD_AUDIO_DAC_INT_MASK \
        (CPLD_AUDIO_DAC_INT_PENDING | CPLD_AUDIO_DAC_INT_ENALBED)
#define CPLD_AUDIO_ADC_INT_MASK \
        (CPLD_AUDIO_ADC_INT_PENDING | CPLD_AUDIO_ADC_INT_ENABLED)

#define CPLD_ALL_ADC_BITS (CPLD_ADC_IRQ_STATUS | \
                           CPLD_ADC_IRQ_ENABLE | \
                           CPLD_ADC_DMA_AUTO | \
                           CPLD_ADC_DMA_ENABLE)

#define CPLD_ALL_DAC_BITS (CPLD_DAC_IRQ_STATUS | \
                           CPLD_DAC_IRQ_ENABLE | \
                           CPLD_DAC_DMA_AUTO | \
                           CPLD_DAC_DMA_ENABLE | \
                           CPLD_DAC_USE_REQ1)

#define CPLD_ALL_AUDIO_BITS (CPLD_ALL_DAC_BITS | CPLD_ALL_DAC_BITS)

#define CPLD_FS_BIT_FIELD 8
#define CPLD_FS_BITS _SBF(CPLD_FS_BIT_FIELD, _BITMASK(4) );
#define CPLD_FS_8000    0
#define CPLD_FS_11025   1
#define CPLD_FS_12000   2
#define CPLD_FS_16000   3
#define CPLD_FS_22050   4
#define CPLD_FS_24000   5
#define CPLD_FS_32000   6
#define CPLD_FS_44100   7
#define CPLD_FS_48000   8


/* misc_stat register status bits */
#define CPLD_MISC_STAT_XGPIO0 _BIT(0)
#define CPLD_MISC_STAT_XGPIO1 _BIT(1)
#define CPLD_MISC_STAT_RI     _BIT(2)
#define CPLD_MISC_STAT_CTS    _BIT(3)
#define CPLD_MISC_STAT_TOUCH  _BIT(4)

#define CPLD_INT_STAT_FIELD 6
#define CPLD_INT_NSOURCES 10
#define CPLD_MISC_STAT_DAC_INT_SOURCE 0
#define CPLD_MISC_STAT_ADC_INT_SOURCE 1
#define CPLD_MISC_STAT_NTOUCH_INT_SOURCE 2
#define CPLD_MISC_STAT_INT_BUTTON_INT_SOURCE 3
#define CPLD_MISC_STAT_LEFT_BUTTON_INT_SOURCE 4
#define CPLD_MISC_STAT_RIGHT_BUTTON_INT_SOURCE 5
#define CPLD_MISC_STAT_UP_BUTTN_INT_SOURCE 6
#define CPLD_MISC_STAT_DOWN_BUTTON_INT_SOURCE 7
#define CPLD_MISC_STAT_CTS1_INT_SOURCE 8
#define CPLD_MISC_STAT_RI1_INT_SOURCE 9

#define CPLD_MISC_STAT_DAC_INT _BIT( \
                              CPLD_MISC_STAT_DAC_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)                                    
#define CPLD_MISC_STAT_ADC_INT _BIT(\
                              CPLD_MISC_STAT_ADC_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)                                    
#define CPLD_MISC_STAT_NTOUCH_INT _BIT( \
                              CPLD_MISC_STAT_NTOUCH_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_INT_BUTTON_INT _BIT( \
                              CPLD_MISC_STAT_INT_BUTTON_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_LEFT_BUTTON_INT _BIT(\
                              CPLD_MISC_STAT_LEFT_BUTTON_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_RIGHT_BUTTON_INT _BIT( \
                              CPLD_MISC_STAT_RIGHT_BUTTON_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_UP_BUTTN_INT _BIT( \
                              CPLD_MISC_STAT_UP_BUTTN_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_DOWN_BUTTON_INT _BIT( \
                              CPLD_MISC_STAT_DOWN_BUTTON_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_CTS1_INT _BIT( \
                              CPLD_MISC_STAT_CTS1_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)
#define CPLD_MISC_STAT_RI1_INT _BIT( \
                              CPLD_MISC_STAT_RI1_INT_SOURCE \
                              + CPLD_INT_STAT_FIELD)

/* gpio_data_dir bits */
#define CPLD_XGPIO0_IS_OUTPUT _BIT(0)
#define CPLD_XGPIO1_IS_OUTPUT _BIT(1)

/* ssp_dev_sel register bits */
#define CPLD_SSPFRM_EEPROM       0
#define CPLD_SSPFRM_TOUCHSCREEN  1
#define CPLD_SSPFRM_EXPANSION    2

/* intr_mask bits */
#define CPLD_INT_MASK_RI      _BIT(5)
#define CPLD_INT_MASK_CTS     _BIT(6)
#define CPLD_INT_MASK_TOUCH   _BIT(7)

/* 
Write to the nio_reg_clk to control these functions on the I/O board.
Note: bits with a lower-case n in the name must be written 0 to active
the function. At reset, all bits in this register are 0.
*/
#define CPLD_PC1_PWR1      _BIT(0)
#define CPLD_nAC97_PWR_EN  _BIT(1)
#define CPLD_PC2_PWR2      _BIT(2)
#define CPLD_PC2_PWR1      _BIT(3)
#define CPLD_PC1_PWR2      _BIT(4)
#define CPLD_nI2S_PWR_EN   _BIT(5)
#define CPLD_nETH_SLEEP    _BIT(6)
#define CPLD_MMC_VCC_EN    _BIT(7)

/**********************************************************************
 * IOCON MemMux register intialization 
 *********************************************************************/

#define IOCON_MEMMUX_INIT   (MEMMUX_MIDQM30 | MEMMUX_MINWE | \
                            MEMMUX_MISDNCS0 | MEMMUX_MISDNCS1 | \
                            MEMMUX_MICKE | MEMMUX_MICLKIO | \
                            MEMMUX_MIDATA_X | \
                            MEMMUX_MICSN3 | MEMMUX_MICSN4 | \
                            MEMMUX_MICSN5 | MEMMUX_MICSN6 | \
                            MEMMUX_MIBLSN2 | MEMMUX_MIBLSN3 )

/***********************************************************************
 * Clock Indexes
 * Caution: these indexes have to be coherent with the equivalent
 * indexes in LH79520_evb.i
 **********************************************************************/

#define RCPC_CLKIDX_DEFAULT     0
#define RCPC_CLKIDX_39_39       1
#define RCPC_CLKIDX_52_39       2
#define RCPC_CLKIDX_52_52       3
#define RCPC_CLKIDX_78_39       4
#define RCPC_CLKIDX_78_52       5
#define RCPC_CLKIDX_78_78       6
#define RCPC_CLKIDX_10_10       7

/***********************************************************************
 * LCD Clock Definitions
 **********************************************************************/

#define LH79520_XTAL_FREQ 14745600

#define LH79520_CLKIN_FREQ 0

#if defined (CLKIN_FREQ_20)
/* JP 37 Open JP38 Open */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 20000000
#endif

#if defined (CLKIN_FREQ_30)
/* JP 37 2-3 JP38 Open */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 30000000
#endif

#if defined (CLKIN_FREQ_31)
/* JP 37 Open JP38 2-3 */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 31250000
#endif

#if defined (CLKIN_FREQ_40)
/* JP 37 1-2 JP38 1-2 */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 40000000
#endif

#if defined (CLKIN_FREQ_50)
/* JP 37 1-2 JP38 2-3 */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 50000000
#endif

#if defined (CLKIN_FREQ_62)
/* JP 37 Open JP38 1-2 */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 62500000
#endif

#if defined (CLKIN_FREQ_80)
/* JP 37 2-3 JP38 2-3 */
#undef LH79520_CLKIN_FREQ
#define LH79520_CLKIN_FREQ 80000000
#endif

/***********************************************************************
 * Refresh timer values for various Clock Indexes
 **********************************************************************/

#define REFTIMER_78     0x480
#define REFTIMER_52     0x320
#define REFTIMER_39     0x270
#define REFTIMER_10     0x80

/* RCPC_CLKSET_XX assume internal oscillator driven by 14.7456 XTAL */ 
#define RCPC_CLKSET_78          RCPC_PRESCALER_DIV4
#define RCPC_CLKSET_52          RCPC_PRESCALER_DIV6
#define RCPC_CLKSET_39          RCPC_PRESCALER_DIV8

/* User can use RCPC->xxxClkPrescale and the following define's will
 * provide the correct mapping to the Sharp_rcpc.h structure element */ 
#define LCDClkPrescale  Spare0Prescale
#define SSPClkPrescale  Spare1Prescale

/* System Clock frequency in Hz */ 
#define RCPC_SYSCLK_FREQ_78     77414400
#define RCPC_SYSCLK_FREQ_52     51609600
#define RCPC_SYSCLK_FREQ_39     38707200

/***********************************************************************
 * Miscellaneous Constants
 **********************************************************************/
#define LH79520_EVB_LED_D9              _BIT(4)
#define LH79520_EVB_LED_D10             _BIT(3)
#define LH79520_EVB_LED_ON              0
#define LH79520_EVB_LED_OFF             1
#define LH79520_EVB_LED_TOGGLE          2

#endif /* LH79520_EVB_H */ 
