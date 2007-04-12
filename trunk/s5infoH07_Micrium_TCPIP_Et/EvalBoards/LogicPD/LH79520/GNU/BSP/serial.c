/*!--------------------------------------------------------------------------
 * \file	serial.c
 * \brief	Contains architecture specific serial input and output routines.
 */
/* © Copyright 2002, Logic Product Development, Inc. All Rights Reserved.
 *
 * NOTICE:
 *  This file contains source code, ideas, techniques, and information
 *  (the Information) which are Proprietary and Confidential Information
 *  of Logic Product Development, Inc.  This Information may not be used
 *  by or disclosed to any third party except under written license, and
 *  shall be subject to the limitations prescribed under license.
 *
 *-------------------------------------------------------------------------*/
#include "serial.h"			/* por alguna razon no marcha */
#include <stdio.h>
#include <stdarg.h>


#define UART_BASE	(0xFFFC0000 + 0x01000)
#define IOCON_BASE	(0xFFFC0000 + 0x25000)
#define RCPCBASE	(0xfffe2000)

/*
 * UART Module Register Structure
 */
typedef struct
{
	volatile unsigned int dr;
	volatile unsigned int rsr_ecr;
	volatile unsigned int reserveduart1[4];
	volatile unsigned int fr;
	volatile unsigned int reserveduart2;
	volatile unsigned int ilpr;
	volatile unsigned int ibrd;
	volatile unsigned int fbrd;
	volatile unsigned int lcr_h;
	volatile unsigned int cr;
	volatile unsigned int ifls;
	volatile unsigned int imsc;
	volatile unsigned int ris;
	volatile unsigned int mis;
	volatile unsigned int icr;
} UARTREGS;

/*
 * IO Configuration Block Structure
 */
typedef struct
{
	volatile unsigned int   memmux;
	volatile unsigned int   lcdmux;
	volatile unsigned int   miscmux;
	volatile unsigned int   dmamux;
	volatile unsigned int   uartmux;
	volatile unsigned int   ssimux;
	volatile unsigned int   scratchreg;
} IOCONREGS;


/*!--------------------------------------------------------------------------
 *
 * \brief	Initializes a debug serial port.
 *
 * \b Purpose:
 *
 *	This function initializes a serial port for communication purposes.
 *
 * \return	none.
 *
 *-------------------------------------------------------------------------*/
void init_serial_port(void)
{
	UARTREGS  *uart;
	IOCONREGS *iocon;
	volatile unsigned int *rcpc_periphclkctrl;
	volatile unsigned int *rcpc_periphclksel;

	uart = (UARTREGS *)UART_BASE;
	iocon= (IOCONREGS *)IOCON_BASE;

	rcpc_periphclkctrl = (volatile unsigned int *)(RCPCBASE + 0x024);
	rcpc_periphclksel  = (volatile unsigned int *)(RCPCBASE + 0x030);

	/*
	 * The Sharp manual says to always disable the UART before making any
	 * changes.
	 */
	uart->cr = 0;

	/*
	 * Enable the selected UART clock in the RCPC. The clock selection is
	 * the internal clock (UART Clock=14.7456x MHz).
	 */
	*rcpc_periphclkctrl = (*rcpc_periphclkctrl	& ~(0x2));
	*rcpc_periphclksel  = (*rcpc_periphclksel	& ~(0x2));

	/*
	 * Set the baud rate to 115200.
	 */
	uart->ibrd =  0x08;

	/*
	 * There is never a fractional part with our clock and any
	 * of the standard baud rates.
	 */
	uart->fbrd = 0;

	/*
	 * Set stop bits, parity, and UART bit control options No break,
	 * FIFOs not enabled, 8-bit word length, stick parity disabled.
	 */
	uart->lcr_h = 0x00000060;

	/*
	 * Clear any pending interrupts for the UART.
	 */
	uart->icr = 0xffffffff;

	/*
	 * Enable UART pins on muxed lines for UART1.
	 */
	iocon->uartmux = (iocon->uartmux | 0x0000000c);

	/*
	 * Disable CTS/RTS flow control, enable receiver/transmitter, enable
	 * the UART.
	 */
	uart->cr = 0x00000301;

	return;

} /* end init_serial_port() */


/*!--------------------------------------------------------------------------
 *
 * \brief	Writes a byte to a debug serial port.
 *
 * \b Purpose:
 *
 *	This function writes a single byte to a serial port. Note, this function
 *	also blocks until room is available in the UART for the byte to be sent.
 *
 * \return	none.
 *
 *-------------------------------------------------------------------------*/
void write_serial_byte(const unsigned char b)
{
	UARTREGS  *uart;
	uart = (UARTREGS *)UART_BASE;

 	while ( 0 == (uart->fr & 0x00000080) )
   	{
   		/*
   		 * Wait for room.
   		 */
   		;
   	}
   	uart->dr = (unsigned int)b;

	return;

} /* end write_serial_byte() */


/*!--------------------------------------------------------------------------
 *
 * \brief	Writes a string to a debug serial port.
 *
 * \b Purpose:
 *
 *	This funcion writes a character string to a serial port.
 *
 * \return	none.
 *
 *-------------------------------------------------------------------------*/
void write_serial_string(const unsigned char *s)
{
	while ( *s )
	{
		write_serial_byte(*s++);
	}

	return;

} /* end write_serial_string() */


/*!--------------------------------------------------------------------------
 *
 * \brief	Reads a byte from a debug serial port.
 *
 * \b Purpose:
 *
 *	This funcion reads a byte from a serial port. This call blocks until a
 *	character appears at the port.
 *
 * \return	An integer with the byte received in the LSB.
 *
 *-------------------------------------------------------------------------*/
int read_serial_byte(void)
{
	unsigned int b;
	UARTREGS  *uart;
	uart = (UARTREGS *)UART_BASE;

	while ( 0 != (uart->fr & 0x10) )
	{
		/*
		 * Wait for a byte to show up.
		 */
		;
	}

	/*
	 * Read the character.
	 */
	b = uart->dr;

	return (b & 0x000000ff);

} /* end read_serial_byte() */

/*
 * End file main.c
 */
