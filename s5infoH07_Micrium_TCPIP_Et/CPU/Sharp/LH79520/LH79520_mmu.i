;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $Workfile:   LH79520_mmu.i  $
; $Revision:   1.2  $
; $Author:   kovitzp  $
; $Date:   01 Apr 2002 09:39:12  $
; 
; Project:  LH79520
;
; Description:
;
;   LH79520 MMU coprocessor definitions
;   (ARM assembly language include file)
;
; $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_mmu.i-arc  $
;
;   Rev 1.2   01 Apr 2002 09:39:12   kovitzp
;Added legal disclaimer.
;
;   Rev 1.1   Feb 11 2002 16:52:58   BarnettH
;Removed TBD's
;
;   Rev 1.0   Dec 05 2001 09:38:58   BarnettH
;Initial revision.
;
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
; COPYRIGHT (C) 2001  SHARP MICROELECTRONICS OF THE AMERICAS INC.
;                        CAMAS, WA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; MMU, Coprocessor Definitions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SYS_CONTROL_CP		CP	15
MMU_CP				CP	15
CACHE_CP			CP	15

; Valid CP15 registers
MMU_REG_ID			CN	0
MMU_REG_CONTROL		CN	1
MMU_REG_TTB			CN	2		; Translation Table Base Address
MMU_REG_DAC			CN	3		; Domain Access Control
MMU_REG_FS			CN	5		; Fault Status
MMU_REG_FA			CN	6		; Fault Address
MMU_REG_CACHE_OP	CN	7
MMU_REG_TLB_OP		CN	8		; Translation Lookaside Buffer
MMU_REG_FCSE_PID	CN	13		; Fast Context Switch Extension
MMU_REG_TRACE_PID	CN	13

; Control register bits
MMU_ENABLE_BIT			EQU (1 << 0)	; MMU Enable
MMU_AF_ENABLE_BIT		EQU (1 << 1)	; Alignment Fault
MMU_UCACHE_ENABLE_BIT	EQU (1 << 2)	; Unified Cache
MMU_WB_ENABLE_BIT		EQU (1 << 3)	; Write Buffer
MMU_S_BIT				EQU (1 << 8)	; System Protection
MMU_R_BIT				EQU (1 << 9)	; ROM Protection
MMU_V_BIT				EQU (1 << 13)	; Exception Vector location

; Convenient EQU's
MMU_ENABLE				EQU (MMU_ENABLE_BIT)
MMU_ENABLE_CACHE		EQU (MMU_ENABLE_BIT:OR:MMU_UCACHE_ENABLE_BIT)
MMU_ENABLE_CACHE_WB		EQU (MMU_ENABLE_BIT:OR:MMU_UCACHE_ENABLE_BIT:OR: \
								MMU_WB_ENABLE_BIT)

; Small page size - 4KB
; Large page size - 64KB
; Section size - 1MB
SMALL_PAGE_SIZE		EQU		(0x400 * 4)
LARGE_PAGE_SIZE		EQU		(0x4000 * 4)
SECTION_SIZE		EQU		(0x40000 * 4)

; Small page tables and Large page tables are the same size,
; 1024 bytes or 256 entries
; Large page tables have each of 16 entries replicated 16 times
; in succeeding memory locations
PAGE_TABLE_SIZE		EQU		(0x100 * 4) ; 256 entries
TRANS_TABLE_SIZE	EQU		(0x1000 * 4); 4096 entries

; N.B.: Translation table must be based on a 16KB boundary.
;       Page tables must be based on 1KB boundaries.

	END
