/**
 *********************************************************************
 *	@file       dma.h
 *	@version    0.1
 *	@author     ELR
 *	@date       24 Avril 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      CerfPDA: DMA declarations
 *
 *  @todo 
 *
 *
 *	Revision History:
 * 
 * 
 *
 *	COPYRIGHT (C) 2006 Eduardo Luis Romero. Université de Sherbrooke.
 *  Québec, Canada.
 *
 *********************************************************************/
#ifndef _ERPDADMA_H_
#define _ERPDADMA_H_


/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

/* DMA Channel 0   registers    */
#define DMA_CH0_BASE 	0xB0000000

#define DMA_DDAR0           *(volatile INT32U*) (DMA_CH0_BASE + 0x00000000)
#define DMA_DCSR0_W1        *(volatile INT32U*) (DMA_CH0_BASE + 0x00000004)
#define DMA_DCSR0_W0        *(volatile INT32U*) (DMA_CH0_BASE + 0x00000008)
#define DMA_DCSR0_RO        *(volatile INT32U*) (DMA_CH0_BASE + 0x0000000C)
#define DMA_DBSA0           *(volatile INT32U*) (DMA_CH0_BASE + 0x00000010)
#define DMA_DBTA0           *(volatile INT32U*) (DMA_CH0_BASE + 0x00000014)
#define DMA_DBSB0           *(volatile INT32U*) (DMA_CH0_BASE + 0x00000018)
#define DMA_DBTB0           *(volatile INT32U*) (DMA_CH0_BASE + 0x0000001C)

/* DMA Channel 1   registers    */
#define DMA_CH1_BASE 	0xB0000020

#define DMA_DDAR1           *(volatile INT32U*) (DMA_CH1_BASE + 0x00000000)
#define DMA_DCSR1_W1        *(volatile INT32U*) (DMA_CH1_BASE + 0x00000004)
#define DMA_DCSR1_W0        *(volatile INT32U*) (DMA_CH1_BASE + 0x00000008)
#define DMA_DCSR1_RO        *(volatile INT32U*) (DMA_CH1_BASE + 0x0000000C)
#define DMA_DBSA1           *(volatile INT32U*) (DMA_CH1_BASE + 0x00000010)
#define DMA_DBTA1           *(volatile INT32U*) (DMA_CH1_BASE + 0x00000014)
#define DMA_DBSB1           *(volatile INT32U*) (DMA_CH1_BASE + 0x00000018)
#define DMA_DBTB1           *(volatile INT32U*) (DMA_CH1_BASE + 0x0000001C)

/* DMA Channel 2   registers    */
#define DMA_CH2_BASE 	0xB0000040

#define DMA_DDAR2           *(volatile INT32U*) (DMA_CH2_BASE + 0x00000000)
#define DMA_DCSR2_W1        *(volatile INT32U*) (DMA_CH2_BASE + 0x00000004)
#define DMA_DCSR2_W0        *(volatile INT32U*) (DMA_CH2_BASE + 0x00000008)
#define DMA_DCSR2_RO        *(volatile INT32U*) (DMA_CH2_BASE + 0x0000000C)
#define DMA_DBSA2           *(volatile INT32U*) (DMA_CH2_BASE + 0x00000010)
#define DMA_DBTA2           *(volatile INT32U*) (DMA_CH2_BASE + 0x00000014)
#define DMA_DBSB2           *(volatile INT32U*) (DMA_CH2_BASE + 0x00000018)
#define DMA_DBTB2           *(volatile INT32U*) (DMA_CH2_BASE + 0x0000001C)


/* DMA Channel 3   registers    */
#define DMA_CH3_BASE 	0xB0000060

#define DMA_DDAR3           *(volatile INT32U*) (DMA_CH3_BASE + 0x00000000)
#define DMA_DCSR3_W1        *(volatile INT32U*) (DMA_CH3_BASE + 0x00000004)
#define DMA_DCSR3_W0        *(volatile INT32U*) (DMA_CH3_BASE + 0x00000008)
#define DMA_DCSR3_RO        *(volatile INT32U*) (DMA_CH3_BASE + 0x0000000C)
#define DMA_DBSA3           *(volatile INT32U*) (DMA_CH3_BASE + 0x00000010)
#define DMA_DBTA3           *(volatile INT32U*) (DMA_CH3_BASE + 0x00000014)
#define DMA_DBSB3           *(volatile INT32U*) (DMA_CH3_BASE + 0x00000018)
#define DMA_DBTB3           *(volatile INT32U*) (DMA_CH3_BASE + 0x0000001C)

/* DMA Channel 4   registers    */
#define DMA_CH4_BASE 	0xB0000080

#define DMA_DDAR4           *(volatile INT32U*) (DMA_CH4_BASE + 0x00000000)
#define DMA_DCSR4_W1        *(volatile INT32U*) (DMA_CH4_BASE + 0x00000004)
#define DMA_DCSR4_W0        *(volatile INT32U*) (DMA_CH4_BASE + 0x00000008)
#define DMA_DCSR4_RO        *(volatile INT32U*) (DMA_CH4_BASE + 0x0000000C)
#define DMA_DBSA4           *(volatile INT32U*) (DMA_CH4_BASE + 0x00000010)
#define DMA_DBTA4           *(volatile INT32U*) (DMA_CH4_BASE + 0x00000014)
#define DMA_DBSB4           *(volatile INT32U*) (DMA_CH4_BASE + 0x00000018)
#define DMA_DBTB4           *(volatile INT32U*) (DMA_CH4_BASE + 0x0000001C)

/* DMA Channel 5   registers    */
#define DMA_CH5_BASE 	0xB00000A0

#define DMA_DDAR5           *(volatile INT32U*) (DMA_CH5_BASE + 0x00000000)
#define DMA_DCSR5_W1        *(volatile INT32U*) (DMA_CH5_BASE + 0x00000004)
#define DMA_DCSR5_W0        *(volatile INT32U*) (DMA_CH5_BASE + 0x00000008)
#define DMA_DCSR5_RO        *(volatile INT32U*) (DMA_CH5_BASE + 0x0000000C)
#define DMA_DBSA5           *(volatile INT32U*) (DMA_CH5_BASE + 0x00000010)
#define DMA_DBTA5           *(volatile INT32U*) (DMA_CH5_BASE + 0x00000014)
#define DMA_DBSB5           *(volatile INT32U*) (DMA_CH5_BASE + 0x00000018)
#define DMA_DBTB5           *(volatile INT32U*) (DMA_CH5_BASE + 0x0000001C)

/* DMA Channel 6   registers    */
#define DMA_CH6_BASE 	0xB00000C0

#define DMA_DDAR6           *(volatile INT32U*) (DMA_CH6_BASE + 0x00000000)
#define DMA_DCSR6_W1        *(volatile INT32U*) (DMA_CH6_BASE + 0x00000004)
#define DMA_DCSR6_W0        *(volatile INT32U*) (DMA_CH6_BASE + 0x00000008)
#define DMA_DCSR6_RO        *(volatile INT32U*) (DMA_CH6_BASE + 0x0000000C)
#define DMA_DBSA6           *(volatile INT32U*) (DMA_CH6_BASE + 0x00000010)
#define DMA_DBTA6           *(volatile INT32U*) (DMA_CH6_BASE + 0x00000014)
#define DMA_DBSB6           *(volatile INT32U*) (DMA_CH6_BASE + 0x00000018)
#define DMA_DBTB6           *(volatile INT32U*) (DMA_CH6_BASE + 0x0000001C)


#endif      /* end of _ERPDADMA_H_ */


