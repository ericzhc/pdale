//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2003 Universite de Sherbrooke All rights reserved.
//
// Module name:
//
//      mcp_sa.h
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

#ifndef MCP_SA_H
#define MCP_SA_H

#include <types.h>

#define MCCR0	(*(volatile u32*) (0x80060000))
#define MCDR0	(*(volatile u32*) (0x80060008))
#define MCDR1	(*(volatile u32*) (0x8006000c))
#define MCDR2	(*(volatile u32*) (0x80060010))
#define MCSR	(*(volatile u32*) (0x80060018))
#define MCCR1	(*(volatile u32*) (0x90060030))

#define MCCR0_MCE	0x00010000	/* MCP Enable   */
#define MCDR2_DATA	0x0000ffff	/* reg. DATA    */
#define MCDR2_W		0x00010000	/* reg. Write   */
#define MCDR2_ADD	0x001e0000	/* reg. ADDress */
#define MCSR_CWC	0x00001000	/* CODEC register Write Completed  */
#define MCSR_CRC	0x00002000	/* CODEC register Read Completed   */

void mcp_enable(void);
void mcp_disable(void);
void mcp_write_codec_register(u8 const reg, u16 const value);
u16 mcp_read_codec_register(u8 const reg);

#endif //MCP_SA_H
