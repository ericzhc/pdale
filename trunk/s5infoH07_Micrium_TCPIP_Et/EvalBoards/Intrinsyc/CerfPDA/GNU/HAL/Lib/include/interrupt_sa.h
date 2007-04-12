//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke All rights reserved.
//
// Module name:
//
//      mcp_sa.h
//
// Description:
//
//      Interfaces with the StrongArm SA1110 interrupt controller.
//
// Author:
//
//      Philippe Mabilleau ing.
//
// Created:
//
//      October 2003
//
////////////////////////////////////////////////////////////////////////////////

#ifndef INTERRUPT_SA_H
#define INTERRUPT_SA_H

#include <types.h>

#define ICIP	(*(volatile u32*) (0x90050000))
#define ICMR	(*(volatile u32*) (0x90050004))
#define ICLR	(*(volatile u32*) (0x90050008))
#define ICFP	(*(volatile u32*) (0x90050010))
#define ICPR	(*(volatile u32*) (0x90050020))
#define ICCR	(*(volatile u32*) (0x9005000c))

#define IC_GPIO(Nb)	        	/* GPIO [0..10]                    */ \
                	(0x00000001 << (Nb))
#define IC_GPIO0	IC_GPIO (0)	/* GPIO  [0]                       */
#define IC_GPIO1	IC_GPIO (1)	/* GPIO  [1]                       */
#define IC_GPIO2	IC_GPIO (2)	/* GPIO  [2]                       */
#define IC_GPIO3	IC_GPIO (3)	/* GPIO  [3]                       */
#define IC_GPIO4	IC_GPIO (4)	/* GPIO  [4]                       */
#define IC_GPIO5	IC_GPIO (5)	/* GPIO  [5]                       */
#define IC_GPIO6	IC_GPIO (6)	/* GPIO  [6]                       */
#define IC_GPIO7	IC_GPIO (7)	/* GPIO  [7]                       */
#define IC_GPIO8	IC_GPIO (8)	/* GPIO  [8]                       */
#define IC_GPIO9	IC_GPIO (9)	/* GPIO  [9]                       */
#define IC_GPIO10	IC_GPIO (10)	/* GPIO [10]                       */
#define IC_GPIO11_27	0x00000800	/* GPIO [11:27] (ORed)             */
#define IC_LCD  	0x00001000	/* LCD controller                  */
#define IC_Ser0UDC	0x00002000	/* Ser. port 0 UDC                 */
#define IC_Ser1SDLC	0x00004000	/* Ser. port 1 SDLC                */
#define IC_Ser1UART	0x00008000	/* Ser. port 1 UART                */
#define IC_Ser2ICP	0x00010000	/* Ser. port 2 ICP                 */
#define IC_Ser3UART	0x00020000	/* Ser. port 3 UART                */
#define IC_Ser4MCP	0x00040000	/* Ser. port 4 MCP                 */
#define IC_Ser4SSP	0x00080000	/* Ser. port 4 SSP                 */
#define IC_DMA(Nb)	        	/* DMA controller channel [0..5]   */ \
                	(0x00100000 << (Nb))
#define IC_DMA0 	IC_DMA (0)	/* DMA controller channel 0        */
#define IC_DMA1 	IC_DMA (1)	/* DMA controller channel 1        */
#define IC_DMA2 	IC_DMA (2)	/* DMA controller channel 2        */
#define IC_DMA3 	IC_DMA (3)	/* DMA controller channel 3        */
#define IC_DMA4 	IC_DMA (4)	/* DMA controller channel 4        */
#define IC_DMA5 	IC_DMA (5)	/* DMA controller channel 5        */
#define IC_OST(Nb)	        	/* OS Timer match [0..3]           */ \
                	(0x04000000 << (Nb))
#define IC_OST0 	IC_OST (0)	/* OS Timer match 0                */
#define IC_OST1 	IC_OST (1)	/* OS Timer match 1                */
#define IC_OST2 	IC_OST (2)	/* OS Timer match 2                */
#define IC_OST3 	IC_OST (3)	/* OS Timer match 3                */
#define IC_RTC1Hz	0x40000000	/* RTC 1 Hz clock                  */
#define IC_RTCAlrm	0x80000000	/* RTC Alarm                       */

#define ICLR_IRQ	0       	/* Interrupt ReQuest               */
#define ICLR_FIQ	1       	/* Fast Interrupt reQuest          */

#define ICCR_DIM	0x00000001	/* Disable Idle-mode interrupt     */
                	        	/* Mask                            */
#define ICCR_IdleAllInt	(ICCR_DIM*0)	/*  Idle-mode All Interrupt enable */
                	        	/*  (ICMR ignored)                 */
#define ICCR_IdleMskInt	(ICCR_DIM*1)	/*  Idle-mode non-Masked Interrupt */
                	        	/*  enable (ICMR used)             */


#define OSMR0	(*(volatile u32*) (0x90000000))
#define OSMR1	(*(volatile u32*) (0x90000004))
#define OSMR2	(*(volatile u32*) (0x90000008))
#define OSMR3	(*(volatile u32*) (0x9000000c))
#define OSCR	(*(volatile u32*) (0x90000010))
#define OSSR	(*(volatile u32*) (0x90000014))
#define OWER	(*(volatile u32*) (0x90000018))
#define OIER	(*(volatile u32*) (0x9000001c))


#endif //INTERRUPT_SA_H
