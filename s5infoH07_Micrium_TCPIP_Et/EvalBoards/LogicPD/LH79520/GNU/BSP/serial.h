/*!--------------------------------------------------------------------------
 * \file	serial.h
 * \brief	This file contains declarations of the serial I/O routines.
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
#ifndef _SERIAL_H_
#define _SERIAL_H_

void init_serial_port(void);
void write_serial_byte(const unsigned char b);
void write_serial_string(const unsigned char *s);
int  read_serial_byte(void);

#endif /* closes #ifndef _SERIAL_H_ */

/*
 * End file serial.h
 */
