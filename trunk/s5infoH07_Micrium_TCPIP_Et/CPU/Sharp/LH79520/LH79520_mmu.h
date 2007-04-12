/**********************************************************************
 *  $Workfile:   LH79520_mmu.h  $
 *  $Revision:   1.3  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:38:06  $
 *
 *  Project: LH79520
 *
 *  Description:
 *      This file contains the manifest constants describing the 
 *      MMU and Cache on the LH79520
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_mmu.h-arc  $
 * 
 *    Rev 1.3   01 Apr 2002 09:38:06   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.2   Jan 04 2002 10:35:08   BarnettH
 * Changed deprecated #ifndef.
 * Moved MMU Init typedefs to *_mmuinit.h
 * 
 *    Rev 1.1   Dec 12 2001 15:20:38   BarnettH
 * Added MMU Initialization typedef's
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

#if !defined LH79520_MMU_H
#define LH79520_MMU_H

#include "SMA_types.h"

/**********************************************************************
 * MMU and CACHE
 * The coprocessor which controls the MMU and Cache is the
 * "System Control Coprocessor"
 *********************************************************************/

/***********************************************************************
 * System Control Coprocessor definitions
 * These symbols are used as arguments for the <coproc> parameter
 * in MCR and MRC instructions
 **********************************************************************/
#define SYS_CONTROL_CP          p15
#define MMU_CP                  p15
#define CACHE_CP                p15

/***********************************************************************
 * Valid CP15 Coprocessor Registers
 * These symbols are used as arguments for the <CRn> parameter
 * in MCR and MRC instructions
 **********************************************************************/
#define MMU_REG_ID              c0
#define MMU_REG_CONTROL         c1
#define MMU_REG_TTB             c2
#define MMU_REG_DAC             c3
#define MMU_REG_FAULT_STATUS    c5
#define MMU_REG_FAULT_ADDRESS   c6
#define MMU_REG_CACHE_OPS       c7
#define MMU_REG_TLB_OPS         c8
#define MMU_REG_FSCE_PID        c13
#define MMU_REG_TRACE_PROCID    c13

/***********************************************************************
 * MMU Control Register Fields
 **********************************************************************/
/* These values may be used as conventional mask (or set) bits */ 
#define MMU_CONTROL_M           _BIT(0)
#define MMU_CONTROL_A           _BIT(1)
#define MMU_CONTROL_C           _BIT(2)
#define MMU_CONTROL_W           _BIT(3)
#define MMU_CONTROL_S           _BIT(8)
#define MMU_CONTROL_R           _BIT(9)
#define MMU_CONTROL_V           _BIT(13)

#define MMU_CONTROL_FIELD_ENABLE    1
#define MMU_CONTROL_FIELD_DISABLE   0

/* MMU enable */ 
#define MMU_ENABLE          _SBF(0,MMU_CONTROL_FIELD_ENABLE)
#define MMU_DISABLE         _SBF(0,MMU_CONTROL_FIELD_DISABLE)
/* Alignment Fault Checking enable */ 
#define MMU_AFC_ENABLE      _SBF(1,MMU_CONTROL_FIELD_ENABLE)
#define MMU_AFC_DISABLE     _SBF(1,MMU_CONTROL_FIELD_DISABLE)
/* Cache enable */ 
#define MMU_CACHE_ENABLE    _SBF(2,MMU_CONTROL_FIELD_ENABLE)
#define MMU_CACHE_DISABLE   _SBF(2,MMU_CONTROL_FIELD_DISABLE)
/* Write Buffer enable */ 
#define MMU_WB_ENABLE       _SBF(3,MMU_CONTROL_FIELD_ENABLE)
#define MMU_WB_DISABLE      _SBF(3,MMU_CONTROL_FIELD_DISABLE)
/* High Exception Vectors enable */ 
#define MMU_LOW_XVECTORS    _SBF(13,0)
#define MMU_HIGH_XVECTORS   _SBF(13,1)

/***********************************************************************
 * Domain Access Control Register Fields
 * There are 16 domains, 0 - 15
 **********************************************************************/
#define MMU_DOMAIN_NONE     0
#define MMU_DOMAIN_CLIENT   1
#define MMU_DOMAIN_MANAGER  3

/* The following macros may be used to set Domain Access Control */ 
/* The range of argument 'n' is 0 -15 */ 
#define MMU_DOMAIN_NO_ACCESS(n)         (MMU_DOMAIN_NONE << ((n)*2))
#define MMU_DOMAIN_CLIENT_ACCESS(n)     (MMU_DOMAIN_CLIENT << ((n)*2))
#define MMU_DOMAIN_MANAGER_ACCESS(n)    (MMU_DOMAIN_MANAGER << ((n)*2))

/***********************************************************************
 * Fault Status Register Fields
 **********************************************************************/
#define MMU_FSR_DOMAIN(n)   (((n)&0xF0) >> 4)
#define MMU_FSR_TYPE(n)     ((n)&0x0F)

/***********************************************************************
 * Translation Table
 **********************************************************************/
/* Number of entries in TT */ 
#define TT_ENTRIES  4096
#define TT_SIZE     (TT_ENTRIES * 4)

/***********************************************************************
 * Page Table
 **********************************************************************/
/* Number of entries in PT */ 
#define PT_ENTRIES  256
#define PT_SIZE     (PT_ENTRIES * 4)

/***********************************************************************
 * Level 1 Descriptor fields 
 * L1D_x fields apply to both section and page descriptor,
 * where applicable
 **********************************************************************/
#define L1D_TYPE_FAULT     _SBF(0,0)
#define L1D_TYPE_PAGE      _SBF(0,0x11) /*includes compatibility bit 4*/
#define L1D_TYPE_SECTION   _SBF(0,0x12) /*includes compatibility bit 4*/
#define L1D_BUFFERABLE     _BIT(2)
#define L1D_CACHEABLE      _BIT(3)
/* Section AP field meaning depends on CP15 Control Reg S and R bits
 * See LH79520 User's Guide */ 
#define L1D_AP_SVC_ONLY    _SBF(10,1)
#define L1D_AP_USR_RO      _SBF(10,2)
#define L1D_AP_ALL         _SBF(10,3)
#define L1D_DOMAIN(n)      _SBF(5,((n)&0x0F))
#define L1D_SEC_BASE_ADDR(n)    _SBF(20,((n)&0xFFF))

/***********************************************************************
 * Level 2 Descriptor fields 
 * L2D_x fields apply to both large page and small page descriptors,
 * where applicable.
 **********************************************************************/
#define L2D_TYPE_FAULT          _SBF(0,0)
#define L2D_TYPE_LARGE_PAGE     _SBF(0,1)
#define L2D_TYPE_SMALL_PAGE     _SBF(0,2)
#define L2D_BUFFERABLE          _BIT(2)
#define L2D_CACHEABLE           _BIT(3)
#define L2D_AP0_SVC_ONLY        _SBF(4,1)
#define L2D_AP0_USR_RO          _SBF(4,2)
#define L2D_AP0_ALL             _SBF(4,3)
#define L2D_AP1_SVC_ONLY        _SBF(6,1)
#define L2D_AP1_USR_RO          _SBF(6,2)
#define L2D_AP1_ALL             _SBF(6,3)
#define L2D_AP2_SVC_ONLY        _SBF(8,1)
#define L2D_AP2_USR_RO          _SBF(8,2)
#define L2D_AP2_ALL             _SBF(8,3)
#define L2D_AP3_SVC_ONLY        _SBF(10,1)
#define L2D_AP3_USR_RO          _SBF(10,2)
#define L2D_AP3_ALL             _SBF(10,3)
#define L2D_SPAGE_BASE_ADDR(n)  _SBF(12,((n)&0xFFFFF)
#define L2D_LPAGE_BASE_ADDR(n)  _SBF(16,((n)&0xFFFF)

#endif /* LH79520_MMU_H */ 
