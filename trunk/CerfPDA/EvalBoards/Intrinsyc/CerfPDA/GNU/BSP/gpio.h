/**
 *********************************************************************
 *	@file       gpio.h
 *	@version    0.1
 *	@author     ELR
 *	@date       24 Avril 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      CerfPDA: GPIO declarations
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
#ifndef _ERPDAGPIO_H_
#define _ERPDAGPIO_H_


/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define GPIO_BASE_ADD   0x90040000
#define GPIO_GPLR       *(volatile INT32U*)(GPIO_BASE_ADD + 0x00000000)             /* GPIO Pin-Level Register              */
#define GPIO_GPDR       *(volatile INT32U*)(GPIO_BASE_ADD + 0x00000004)             /* GPIO Pin Direction Register          */
#define GPIO_GPSR       *(volatile INT32U*)(GPIO_BASE_ADD + 0x00000008)             /* GPIO Pin Output Set Register         */
#define GPIO_GPCR       *(volatile INT32U*)(GPIO_BASE_ADD + 0x0000000C)             /* GPIO Pin Output Clear Register       */
#define GPIO_GRER       *(volatile INT32U*)(GPIO_BASE_ADD + 0x00000010)             /* GPIO Rising-Edge Detect Register     */
#define GPIO_GFER       *(volatile INT32U*)(GPIO_BASE_ADD + 0x00000014)             /* GPIO Falling-Edge Detect Register    */
#define GPIO_GEDR       *(volatile INT32U*)(GPIO_BASE_ADD + 0x00000018)             /* GPIO Edge Detect Status Register     */
#define GPIO_GAFR       *(volatile INT32U*)(GPIO_BASE_ADD + 0x0000001C)             /* GPIO Alternate Function Register     */

#define P
#endif      /* end of _ERPDAGPIO_H_ */


