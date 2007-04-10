//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke
//
// Module name:
//
//      lcd_sa.h
//
// Description:
//
//      Low level LCD Controler access
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

#ifndef LCD_SA_H
#define LCD_SA_H

#include <types.h>

#define CPLD_LCD (*(volatile u8*) (0x40000000 + 02))

#define LCCR0	(*(volatile u32*) (0xB0100000))
#define LCSR	(*(volatile u32*) (0xB0100004))
#define DBAR1	(*(volatile u32*) (0xB0100010))
#define DCAR1	(*(volatile u32*) (0xB0100014))
#define DBAR2	(*(volatile u32*) (0xB0100018))
#define DCAR2	(*(volatile u32*) (0xB010001c))
#define LCCR1	(*(volatile u32*) (0xB0100020))
#define LCCR2	(*(volatile u32*) (0xB0100024))
#define LCCR3	(*(volatile u32*) (0xB0100028))

void init_lcd(void);
void set_lcd_brightness(int);
void disable_lcd(void);

extern u16 frame_buffer[];

#endif //LCD_SA_H
