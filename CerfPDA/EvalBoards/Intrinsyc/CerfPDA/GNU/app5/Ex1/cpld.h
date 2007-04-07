/**
 *********************************************************************
 *	@file       cpld.h
 *	@version    0.1
 *	@author     ELR
 *	@date       24 Avril 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      CerfPDA: CPLD declarations
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
#ifndef _ERPDACPLD_H_
#define _ERPDACPLD_H_



/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define CERF_PDA_CPLD_BASE_ADD          0x40000000
#define CERF_PDA_CPLD_STATUS            *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x00)    /* Power status (read only)             */
#define CERF_PDA_CPLD_CLR_IRQ           *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x01)    /* Write to clear interrupts            */
#define CERF_PDA_CPLD_LCD_BRIGHT        *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x02)    /* LCD brightness control               */
#define CERF_PDA_CPLD_AUDIO_FSMP        *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x04)    /* Audio sampling frecuency (256x)      */
#define CERF_PDA_CPLD_KEYB_SCNLOW       *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x06)    /* Keyboard scan out low nibble         */
#define CERF_PDA_CPLD_BATT_FAULT        *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x08)    /* Write to set Battery fault           */
#define CERF_PDA_CPLD_KEYB_SCNHIGH      *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x0a)    /* Keyboard scan out high nibble        */
#define CERF_PDA_CPLD_AUDIO_ENA         *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0x0c)    /* Sound enable                         */
#define CERF_PDA_CPLD_AUDIO_LRSYNC      *(INT8U*)(CERF_PDA_CPLD_BASE_ADD + 0X0e)    /* Sound left-righ reset (sync)         */
#define KEYBLINE      *(INT32U*)(0x90040000)    /* Sound left-righ reset (sync)         */


#endif      /* end of _ERPDACPLD_H_ */


