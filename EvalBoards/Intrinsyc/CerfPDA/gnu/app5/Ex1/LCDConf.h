/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : LCDConf.h
Purpose     : Sample configuration file
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

/*********************************************************************
*
*                   General configuration of LCD
*
**********************************************************************
*/

#define LCD_XSIZE             240  /* X-resolution of LCD */
#define LCD_YSIZE             320  /* Y-resolution of LCD */

#define LCD_BITSPERPIXEL       16
#define LCD_FIXEDPALETTE    44412
#define LCD_MAX_LOG_COLORS   4096

#define LCD_CONTROLLER       1300
#define LCD_SWAP_BYTE_ORDER     1
#define LCD_SWAP_RB             1
#define LCD_ALLOW_NON_OPTIMIZED_MODE  1

/*********************************************************************
*
*                   Full bus configuration
*
**********************************************************************
*/

extern unsigned short frame_buffer[];

#define LCD_READ_MEM(Off)        frame_buffer[16+Off]
#define LCD_WRITE_MEM(Off,Data)  frame_buffer[16+Off]=Data

#define LCD_INIT_CONTROLLER()    init_lcd()
#define LCD_OFF()                set_lcd_brightness(0)
#define LCD_ON()                 set_lcd_brightness(15)

#endif /* LCDCONF_H */
