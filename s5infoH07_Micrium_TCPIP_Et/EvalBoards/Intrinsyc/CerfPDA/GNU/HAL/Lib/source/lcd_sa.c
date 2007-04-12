//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke
//
// Module name:
//
//      lcd_sa.c
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

#include <lcd_sa.h>
#include <ucb1200.h>

////////////////////////////////////////////////////////////////////////////////
// init_lcd
// PURPOSE: Initializes the LCD controler, enable the display and set the backlight.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
init_lcd(void)
{
	int i;
	LCCR0 = 0x38;
	LCSR = 0xfff;
	LCCR1 = (240 - 16) | (5 << 10) | (9 << 16) | (61 << 24);
	LCCR2 = 320 - 1;
	LCCR3 = 25 | (70 << 8);
	DBAR1 = (u32) &frame_buffer;
	frame_buffer[0] = 0x2000;
	for (i=1; i<=15; i++) frame_buffer[i] = 0;
	for (i=16; i <= 16+(320*240); i++) frame_buffer[i] = 0x0000;
	LCCR0 = 0x39;
	ucb1200_init();
	ucb1200_enable_lcd();
	set_lcd_brightness(15);
}

////////////////////////////////////////////////////////////////////////////////
// set_lcd_brightness
// PURPOSE: Set the LCD backlight intensity.
// PARAMS:  (IN) int - brightness level 0 to 15.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
set_lcd_brightness(int ilevel)
{
	CPLD_LCD = ilevel;
}

////////////////////////////////////////////////////////////////////////////////
// disable_lcd
// PURPOSE: Turn off the backlight, disable the display and stop the LCD controler.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
disable_lcd(void)
{
	set_lcd_brightness(0);
	ucb1200_disable_lcd();
	LCCR0 = 0x38;
}
