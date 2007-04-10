//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke All rights reserved.
//
// Module name:
//
//      mcp_sa.c
//
// Description:
//
//      Interfaces with the StrongArm SA1110 MCP hardware.
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

#include <mcp_sa.h>

////////////////////////////////////////////////////////////////////////////////
// mcp_enable
// PURPOSE: Initializes and enable the MCP serial port.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
mcp_enable(void)
{
	MCSR = 0xffffffff;
	MCCR0 |= MCCR0_MCE;
}

////////////////////////////////////////////////////////////////////////////////
// mcp_disable
// PURPOSE: Disable the MCP serial port.
// PARAMS:  None.
// RETURNS: Nothing
////////////////////////////////////////////////////////////////////////////////
void
mcp_disable(void)
{
	MCCR0 &= ~MCCR0_MCE;
}

////////////////////////////////////////////////////////////////////////////////
// mcp_write_codec_register
// PURPOSE: write into a codec register.
// PARAMS:  (IN) u8 - register number.
// PARAMS:  (IN) u16 - register value.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
mcp_write_codec_register(u8 const reg, u16 const value)
{
	MCDR2 = (reg << 17) | MCDR2_W | value;
	while (!(MCSR & MCSR_CWC));
}

////////////////////////////////////////////////////////////////////////////////
// mcp_read_codec_register
// PURPOSE: read codec register.
// PARAMS:  (IN) u8 - register number.
// RETURNS: u16 - register value.
////////////////////////////////////////////////////////////////////////////////
u16
mcp_read_codec_register(u8 const reg)
{
	MCDR2 = reg << 17;
	while (!(MCSR & MCSR_CRC));
	return MCDR2 & MCDR2_DATA;
}
