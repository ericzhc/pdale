/**
 *********************************************************************
 *	@file       ssp.h
 *	@version    0.1
 *	@author     ELR
 *	@date       24 Avril 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      CerfPDA: SSP declarations
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
#ifndef _ERPDASSP_H_
#define _ERPDASSP_H_


/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define CERF_PDA_SSP_BASE_ADD           0x80070000
#define CERF_PDA_SSP_CNTRL0             *(INT32U*)(CERF_PDA_SSP_BASE_ADD + 0x60)    /* Power status (read only)             */
#define CERF_PDA_SSP_CNTRL1             *(INT32U*)(CERF_PDA_SSP_BASE_ADD + 0x64)    /* Write to clear interrupts            */
#define CERF_PDA_SSP_DATA               *(INT32U*)(CERF_PDA_SSP_BASE_ADD + 0x6C     /* LCD brightness control               */
#define CERF_PDA_SSP_STATUS             *(INT32U*)(CERF_PDA_SSP_BASE_ADD + 0x74)    /* Audio sampling frecuency (256x)      */

/* SSP / GPIO pin definition  */
#define GPIO_SSP_TXD	    0x00000400                                	            /*  SSP Transmit Data (O)               */
#define GPIO_SSP_RXD	    0x00000800                                              /*  SSP Receive Data (I)                */
#define GPIO_SSP_SCLK	    0x00001000                                              /*  SSP Sample CLocK (O)                */
#define GPIO_SSP_SFRM	    0x00002000                                	            /*  SSP Sample FRaMe (O)                */
#define GPIO_SSP_CLK	    0x00080000                                	            /*  SSP external CLocK (I)              */ 

#endif      /* end of _ERPDASSP_H_ */


