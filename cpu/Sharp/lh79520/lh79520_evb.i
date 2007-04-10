;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $Workfile:   lh79520_evb.i  $
; $Revision:   1.11  $
; $Author:   kovitzp  $
; $Date:   01 Apr 2002 10:04:14  $
;
; Project:  LH79520 Evaluation Board
;
; Description:
;       LH79520 EVB (Platform) definitions
;       (ARM assembly language include file)
;
; Pre-defines:
;       REMAP:
;       REMAP must be {0|1|2}.Set this constant to the desired value
;       with an assembler SETA pre-define, e.g., "REMAP SETA 0x0"
;
; $Log:   P:/PVCS6_6/archives/SOC/Platforms/LH79520/lh79520_evb.i-arc  $
;
;   Rev 1.11   01 Apr 2002 10:04:14   kovitzp
;Added legal disclaimer.
;
;   Rev 1.10   Mar 27 2002 11:21:10   MaysR
;Added CPLD label for seven segments.
;
;   Rev 1.9   Mar 11 2002 20:29:58   BarnettH
;Added BIT addresses
;
;   Rev 1.8   Mar 05 2002 13:48:08   BarnettH
;Changed starting stacks to the top of SDRAM.  This assumes Translation Tables are initialized with tt_init_basic.
;Corrected error in stack setup.
;Added "NO INIT" Dip Switch setting
;
;   Rev 1.7   Feb 25 2002 17:25:18   BarnettH
;Added needed CPLD and SMC constants
;
;   Rev 1.6   Feb 20 2002 17:51:20   BarnettH
;Added IOCON, GPIO0 equates
;
;   Rev 1.5   Feb 12 2002 08:55:58   MaysR
;Added CPLD register addresses, for reading DIP switches at startup.
;
;   Rev 1.4   Jan 30 2002 13:40:08   BarnettH
;Changed include file name to LH79520_mmu.i
;
;   Rev 1.3   Jan 16 2002 21:05:22   BarnettH
;Removed preliminary status
;
;   Rev 1.2   Aug 16 2001 18:00:18   BarnettH
;Preliminary stacks are moved to IRAM
;
;   Rev 1.1   Aug 06 2001 15:07:14   BarnettH
;Second preliminary release
;
;   Rev 1.0   Aug 03 2001 14:17:54   BarnettH
;Initial revision.
;
; SHARP MICROELECTRONICS OF THE AMERICAS MAKES NO REPRESENTATION
; OR WARRANTIES WITH RESPECT TO THE PERFORMANCE OF THIS SOFTWARE,
; AND SPECIFICALLY DISCLAIMS ANY RESPONSIBILITY FOR ANY DAMAGES, 
; SPECIAL OR CONSEQUENTIAL, CONNECTED WITH THE USE OF THIS SOFTWARE.
;
; SHARP MICROELECTRONICS OF THE AMERICAS PROVIDES THIS SOFTWARE SOLELY 
; FOR THE PURPOSE OF SOFTWARE DEVELOPMENT INCORPORATING THE USE OF A 
; SHARP MICROCONTROLLER OR SYSTEM-ON-CHIP PRODUCT. USE OF THIS SOURCE
; FILE IMPLIES ACCEPTANCE OF THESE CONDITIONS.
;
;  COPYRIGHT (C) 2001  SHARP MICROELECTRONICS OF THE AMERICAS INC.
;                         CAMAS, WA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; SPECIALIZED INCLUDES
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    INCLUDE LH79520_mmu.i

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; LH79520 xPSR bits
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

MODE_USR        EQU     0x010
MODE_FIQ        EQU     0x011
MODE_IRQ        EQU     0x012
MODE_SVC        EQU     0x013
MODE_ABORT      EQU     0x017
MODE_UNDEF      EQU     0x01b
MODE_SYSTEM     EQU     0x01F
I_MASK          EQU     (1 << 7)  ; 0x080
F_MASK          EQU     (1 << 6)  ; 0x040
T_MASK          EQU     (1 << 5)  ; 0x020

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; EVB MEMORY (Physical)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Memory Mapped Registers
REG_AHB_PHYS_BASE       EQU 0xFFFF0000
REG_APB_PHYS_BASE       EQU 0xFFFC0000

; Synchronous DRAM
SDRAM_PHYS_BASE         EQU 0x20000000
SDRAM_PHYS_SIZE         EQU 0x04000000  ; 64MB
SDRAM_PHYS_END          EQU SDRAM_PHYS_BASE + SDRAM_PHYS_SIZE

; Static RAM
SRAM_PHYS_BASE          EQU 0x44000000
SRAM_PHYS_SIZE          EQU 0x00200000  ; 2MB
SRAM_PHYS_END           EQU (SRAM_PHYS_BASE + SRAM_PHYS_SIZE)

; Internal RAM
IRAM_PHYS_BASE          EQU 0x60000000
IRAM_PHYS_SIZE          EQU 0x8000  ;  32KB
IRAM_PHYS_END           EQU IRAM_PHYS_BASE + IRAM_PHYS_SIZE

; Flash ROM
FLASH_PHYS_BASE         EQU 0x40000000
FLASH_PHYS_SIZE         EQU 0x00400000  ;  4MB
FLASH_PHYS_END          EQU FLASH_PHYS_BASE + FLASH_PHYS_SIZE

;   IF REMAP = 0x0
FLASH_MIRROR_BASE       EQU 0x0
;   IF REMAP = 0x1
SDRAM_MIRROR_BASE       EQU 0x0
;   IF REMAP = 0x2
IRAM_MIRROR_BASE        EQU 0x0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BootROM Load Addresses
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Translation Tables and Page Tables will be built from executable
; code at startup.

; Load and run applications at 0x8000 (32K) base
EVB520_APPL_BASE_LADDR  EQU     (0x8000)

; Translation Table Base Address
TT_BASE_LADDR           EQU     (EVB520_APPL_BASE_LADDR - TRANS_TABLE_SIZE)

; Page Tables Base Addresses for four page tables
PT1_BASE                EQU     (TT_BASE_LADDR - PAGE_TABLE_SIZE)
PT2_BASE                EQU     (PT1_BASE - PAGE_TABLE_SIZE)
PT3_BASE                EQU     (PT2_BASE - PAGE_TABLE_SIZE)
PT4_BASE                EQU     (PT3_BASE - PAGE_TABLE_SIZE)
PAGE_TABLES_BASE_LADDR  EQU     (PT4_BASE)
PAGE_TABLES_SIZE        EQU     (TT_BASE_LADDR - PAGE_TABLES_BASE_LADDR)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Translation and Page Table Addresses
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

TT_BASE_XADDR           EQU     (SRAM_PHYS_END - TRANS_TABLE_SIZE)
PAGE_TABLES_BASE_XADDR  EQU     (TT_BASE_XADDR - PAGE_TABLES_SIZE)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Load Supervisor stack setup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Size will be as needed, calling depth is expected to be minimal
; before Primary stack setup
; Stack usage is assumed to be Full Descending
; Leave some Data Store at top of IRAM
DATA_STORE      EQU 0x100
SVC_STACK_IRAM  EQU (IRAM_PHYS_BASE + IRAM_PHYS_SIZE - DATA_STORE)
SVC_STACK_SDRAM EQU (0x0 + SDRAM_PHYS_SIZE)
; Boot Type
BOOT_TYPE_ADDR  EQU SVC_STACK_IRAM
; Status Register
SRVAL_ADDR      EQU SVC_STACK_IRAM + 4
; DIP SWITCH (S1) Status Address
DIPSW_ADDR      EQU SVC_STACK_IRAM + 8
; Location to store BIT image beginning address in Flash
BIT_IMAGE_BEGIN_ADDR        EQU    SVC_STACK_IRAM + 12
; Location to store BIT image ending address in Flash
BIT_IMAGE_END_ADDR      EQU    SVC_STACK_IRAM + 16
; Location to store BIT entry point address in Flash
BIT_ENTRY_POINT_ADDR    EQU SVC_STACK_IRAM + 20

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; BIT constants
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BIT_IRAM_BASE   EQU 0x60000000
BIT_ENTRY_POINT EQU 0x60000000
BIT_IRAM_LENGTH EQU (0x8000 - 0x100)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Primary stack setup
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SVC_STACK_SIZE      EQU     1024    ; primary running stack for Supervisor
IRQ_STACK_SIZE      EQU     1024    ; primary running stack for IRQ
UNDEF_STACK_SIZE    EQU     1024    ; may be used by debugger or SWI
FIQ_STACK_SIZE      EQU     256     ; primary running stack for FIQ
ABORT_STACK_SIZE    EQU     256     ; Memory error handler stack
SYSTEM_STACK_SIZE   EQU     1024    ; not used if User Mode not entered.
; In SDRAM below page tables and above BootROM code
; Caution! code in bottom 32K may grow to overrun lower stacks
; This includes OS exception vector code
; Monitor code size in lower INITIALIZE area on load map
;SVC_STACK_INIT      EQU     SVC_STACK_IRAM
SVC_STACK_INIT      EQU     SVC_STACK_SDRAM
IRQ_STACK_INIT      EQU     (SVC_STACK_INIT - SVC_STACK_SIZE)
UNDEF_STACK_INIT    EQU     (IRQ_STACK_INIT - IRQ_STACK_SIZE)
FIQ_STACK_INIT      EQU     (UNDEF_STACK_INIT - UNDEF_STACK_SIZE)
ABORT_STACK_INIT    EQU     (FIQ_STACK_INIT - FIQ_STACK_SIZE)
SYSTEM_STACK_INIT   EQU     (ABORT_STACK_INIT - ABORT_STACK_SIZE)
END_STACK_INIT      EQU     (SYSTEM_STACK_INIT - SYSTEM_STACK_SIZE)


; Miscellaneous LH79520 registers
; IOCON
IOCON_REG_BASE                  EQU     0xFFFE5000
IOCON_MEMMUX_OFFSET             EQU     0x00
IOCON_LCDMUX_OFFSET             EQU     0x04
IOCON_MISCMUX_OFFSET            EQU     0x08
IOCON_DMAMUX_OFFSET             EQU     0x0A
IOCON_UARTMUX_OFFSET            EQU     0x10
IOCON_SSPMUX_OFFSET             EQU     0x14
; GPIO
GPIO0_REG_BASE                  EQU     0xFFFDF000
GPIOPADR_OFFSET                 EQU     0x00
GPIOPADDR_OFFSET                EQU     0x08
; WDT
WDT_REG_BASE                    EQU     0xFFFE3000
WDT_WDCTLR_OFFSET               EQU     0x0
; CPLD
CPLD_REG_BASE                   EQU     0x48000000
CPLD_CPUDIPSW_OFFSET            EQU     0x3C
CPLD_CPUDIPSW_PROGMASK          EQU     0x0F
CPLD_CPUDIPSW_CACHE_BIT         EQU     0x80
CPLD_CPUDIPSW_BIT_RUN           EQU     0x0F
CPLD_CPUDIPSW_IDLE_RUN          EQU     0x07
CPLD_CPUDIPSW_ANGEL_RUN         EQU     0x0D
CPLD_CPUDIPSW_DEMO_RUN          EQU     0x0E
CPLD_CPUDIPSW_USER_RUN          EQU     0x0B
CPLD_CPUDIPSW_MICROCOS_RUN      EQU     0x06
CPLD_CPUDIPSW_NO_INIT_RUN       EQU     0x00
CPLD_SEVEN_SEG                  EQU     0x24
; SMC
SMC_REG_BASE                    EQU     0xFFFF1000
SMC_BCR2_OFFSET                 EQU     0x08
SMC_BCR2_INIT                   EQU     0x13000C20
; RCPC
RCPC_REG_BASE                   EQU     0xFFFE2000
RCPC_CONTROL_OFFSET             EQU     0x00
RCPC_REMAP_OFFSET               EQU     0x08
RCPC_SOFTRESET_OFFSET           EQU     0x0C
RCPC_RESETSTATUS_OFFSET         EQU     0x10
RCPC_RESETSTATUS_CLEAR_OFFSET   EQU     0x14
RCPC_HCLKCLK_PRESCALE_OFFSET    EQU     0x18
RCPC_CPUCLK_PRESCALE_OFFSET     EQU     0x1C
RCPC_PERIPHCLKCTRL_OFFSET       EQU     0x24
RCPC_AHBCLKCTRL_OFFSET          EQU     0x2C
RCPC_PERIPHCLKSELECT_OFFSET     EQU     0x30
RCPC_INTCONFIG                  EQU     0x80
RCPC_INTCLEAR                   EQU     0x84
RCPC_CORECLKCONFIG_OFFSET       EQU     0x88

RCPC_CPUCLK_PRESCALE_78         EQU     0x2
RCPC_CPUCLK_PRESCALE_52         EQU     0x3
RCPC_CPUCLK_PRESCALE_39         EQU     0x4
RCPC_HCLK_PRESCALE_52           EQU     0x3
RCPC_HCLK_PRESCALE_39           EQU     0x4
RCPC_CPUCLK_PRESCALE_DEFAULT    EQU     RCPC_CPUCLK_PRESCALE_52
RCPC_HCLK_PRESCALE_DEFAULT      EQU     RCPC_HCLK_PRESCALE_52

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Clock Indexes
; Caution: these indexes have to be coherent with the equivalent indexes
; included with associated 'C' modules
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

RCPC_CLKIDX_DEFAULT     EQU   0
RCPC_CLKIDX_39_39       EQU   1
RCPC_CLKIDX_52_39       EQU   2
RCPC_CLKIDX_52_52       EQU   3
RCPC_CLKIDX_78_39       EQU   4
RCPC_CLKIDX_78_52       EQU   5
RCPC_CLKIDX_78_78       EQU   6
RCPC_CLKIDX_10_10       EQU   7

;
; END of INCLUDE file
;
    END
