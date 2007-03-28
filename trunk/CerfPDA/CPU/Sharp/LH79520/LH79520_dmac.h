/**********************************************************************
 *  $Workfile:   LH79520_dmac.h  $
 *  $Revision:   1.3  $
 *  $Author:   kovitzp  $
 *  $Date:   01 Apr 2002 09:32:24  $
 *
 *  Project: LH79520 headers
 *
 *  Description:
 *      This file contains the structure definitions and manifest
 *      constants for component:
 *
 *          DMA Controller 
 *
 *  References:
 *      (1) Sharp LH79520 Universal Microcontroller User's Guide,
 *      Version 3.0 Sharp Microelectronics of the Americas, Inc.
 *
 *  Revision History:
 *  $Log:   P:/PVCS6_6/archives/SOC/LH79520/Processor/LH79520_dmac.h-arc  $
 * 
 *    Rev 1.3   01 Apr 2002 09:32:24   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.2   Jan 18 2002 09:50:36   KovitzP
 * added DMAC_ALL_FLAGS
 * 
 *    Rev 1.1   Jan 02 2002 09:55:42   BarnettH
 * Revised register manifest constants
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

#if !defined LH79520_DMAC_H
#define LH79520_DMAC_H

#include "SMA_types.h"

/***********************************************************************
 * DMAC Module Register Structures
 **********************************************************************/ 

typedef struct 
{
    volatile UNS_32 sourcelo; /* Source base addr, lo 16 bits */
    volatile UNS_32 sourcehi; /* Source base addr, hi 16 bits */
    volatile UNS_32 destlo;   /* Dest base addr, lo 16 bits */ 
    volatile UNS_32 desthi;   /* Dest base addr, hi 16 bits */ 
    volatile UNS_32 max;      /* Maximum Count */       
    volatile UNS_32 ctrl;     /* Control */ 
    volatile UNS_32 socurrhi; /* Current src addr, hi 16 bits*/ 
    volatile UNS_32 socurrlo; /* Current src addr, lo 16 bits*/ 
    volatile UNS_32 decurrhi; /* Curr dest addr, hi 16 bits*/ 
    volatile UNS_32 decurrlo; /* Curr src addr, lo 16 bits*/ 
    volatile UNS_32 tcnt;     /* Terminal Count */      
} DATASTREAMREGS;

typedef struct 
{
    DATASTREAMREGS  stream0;    /* Data Stream 0 */ 
    volatile UNS_32 reserved0[5];
    DATASTREAMREGS  stream1;    /* Data Stream 1 */ 
    volatile UNS_32 reserved1[5];
    DATASTREAMREGS  stream2;    /* Data Stream 2 */ 
    volatile UNS_32 reserved2[5];
    DATASTREAMREGS  stream3;    /* Data Stream 3 */ 
    volatile UNS_32 reserved3;
    volatile UNS_32 mask;
    volatile UNS_32 clear;
    volatile UNS_32 status;
    volatile UNS_32 reserved4;
} DMACREGS;

/***********************************************************************
 * ctrl - Control Register Bit Field
 **********************************************************************/ 

#define DMAC_CTRL_ENABLE            _BIT(0)     /* Enable bit */ 
#define DMAC_CTRL_SOINC             _BIT(1)     /* Source Reg inc.bit */ 
#define DMAC_CTRL_DEINC             _BIT(2)     /* Dest Reg inc.bit */ 
/* Source Size */ 
#define DMAC_CTRL_SOSIZE_1BYTE      _SBF(3,0)
#define DMAC_CTRL_SOSIZE_2BYTE      _SBF(3,1)
#define DMAC_CTRL_SOSIZE_4BYTE      _SBF(3,2)
/* Destination Size */ 
#define DMAC_CTRL_DESIZE_1BYTE      _SBF(7,0)
#define DMAC_CTRL_DESIZE_2BYTE      _SBF(7,1)
#define DMAC_CTRL_DESIZE_4BYTE      _SBF(7,2)
/* Peripheral Burst Sizes */ 
#define DMAC_CTRL_SOBURST_SINGLE    _SBF(5,0)   /* Single */ 
#define DMAC_CTRL_SOBURST_4INC      _SBF(5,1)   /* 4 incrementing */ 
#define DMAC_CTRL_SOBURST_8INC      _SBF(5,2)   /* 8 incrementing */ 
#define DMAC_CTRL_SOBURST_16INC     _SBF(5,3)   /* 16 incrementing */ 
/* Address Modes */ 
#define DMAC_CTRL_ADDR_MODE_WRAP    _SBF(9,0)
#define DMAC_CTRL_ADDR_MODE_INCR    _SBF(9,1)

#define DMAC_CTRL_MEM2MEM           _BIT(11)    /* Memory to Memory */ 
/* Direction */ 
#define DMAC_CTRL_PERIPH_SOURCE     _SBF(13,0)
#define DMAC_CTRL_PERIPH_DEST       _SBF(13,1)

/***********************************************************************
 * mask - Mask Register Bit Fields
 * clear - Clear Register Bit Fields
 * status - Clear Register Bit Fields
 * DMAC_xN_BIT are the bit positions
 * Writing DMAC_xN to mask register enables corresponding interrupt
 * Writing DMAC_xN to clear register disables corresponding interrupt
 * AND'ing DMAC_xN with status register yields status
 * Note: "ACTIVEx" constants are only applicable to Status Register
 **********************************************************************/ 
#define DMAC_EOT0_BIT    0     /* Stream 0 End of Transfer Flag */ 
#define DMAC_EOT1_BIT    1     /* Stream 1 End of Transfer Flag */ 
#define DMAC_EOT2_BIT    2     /* Stream 2 End of Transfer Flag */ 
#define DMAC_EOT3_BIT    3     /* Stream 3 End of Transfer Flag */ 
#define DMAC_ERROR0_BIT  4     /* Stream 0 Error Flag */
#define DMAC_ERROR1_BIT  5     /* Stream 1 Error Flag */ 
#define DMAC_ERROR2_BIT  6     /* Stream 2 Error Flag */ 
#define DMAC_ERROR3_BIT  7     /* Stream 3 Error Flag */ 
#define DMAC_ACTIVE0_BIT 8     /* Stream 0 Active */ 
#define DMAC_ACTIVE1_BIT 9     /* Stream 1 Active */ 
#define DMAC_ACTIVE2_BIT 10    /* Stream 2 Active */ 
#define DMAC_ACTIVE3_BIT 11    /* Stream 3 Active */ 


/* Stream 0 End of Transfer Flag */ 
#define DMAC_EOT0    _BIT(DMAC_EOT0_BIT)
/* Stream 1 End of Transfer Flag */     
#define DMAC_EOT1    _BIT(DMAC_EOT1_BIT)
/* Stream 2 End of Transfer Flag */ 
#define DMAC_EOT2    _BIT(DMAC_EOT2_BIT)
/* Stream 3 End of Transfer Flag */ 
#define DMAC_EOT3    _BIT(DMAC_EOT3_BIT)
#define DMAC_ERROR0  _BIT(DMAC_ERROR0_BIT)  /* Stream 0 Error Flag */
#define DMAC_ERROR1  _BIT(DMAC_ERROR1_BIT)  /* Stream 1 Error Flag */ 
#define DMAC_ERROR2  _BIT(DMAC_ERROR2_BIT)  /* Stream 2 Error Flag */ 
#define DMAC_ERROR3  _BIT(DMAC_ERROR3_BIT)  /* Stream 3 Error Flag */ 
#define DMAC_ACTIVE0 _BIT(DMAC_ACTIVE0_BIT) /* Stream 0 Active */ 
#define DMAC_ACTIVE1 _BIT(DMAC_ACTIVE1_BIT) /* Stream 1 Active */ 
#define DMAC_ACTIVE2 _BIT(DMAC_ACTIVE2_BIT) /* Stream 2 Active */ 
#define DMAC_ACTIVE3 _BIT(DMAC_ACTIVE3_BIT) /* Stream 3 Active */ 

#define DMAC_ALL_FLAGS (DMAC_EOT0 | DMAC_EOT1 | DMAC_EOT2 | DMAC_EOT3 \
                        | DMAC_ERROR0 | DMAC_ERROR1 | DMAC_ERROR2 \
                        | DMAC_ERROR3)

#endif /* LH79520_DMAC_H */ 
