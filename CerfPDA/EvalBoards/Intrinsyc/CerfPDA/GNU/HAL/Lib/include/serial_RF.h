//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2004 University of Sherbrooke All rights reserved.
//
// Module name:
//
//      serial_RF.h
//
// Description:
//
//      Interfaces with the StrongArm SA1110 serial port 1 hardware.
//
// Author:
//
//      Mike Kirkland
//	Philippe Mabilleau ing.
//
// Created:
//
//      DEcember 2003
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SERIAL_RF_H
#define SERIAL_RF_H

#include <types.h>

#ifndef SERIAL_H
#define SERIAL_BAUD_9600   (0x00000017)
#define SERIAL_BAUD_19200  (0x0000000B)
#define SERIAL_BAUD_38400  (0x00000005)
#define SERIAL_BAUD_57600  (0x00000003)
#define SERIAL_BAUD_115200 (0x00000001)
#endif

void init_serial_rf(u32 baud);
void output_byte_serial_rf(char byte);
int input_byte_serial_rf(char *byte);
void set_data_rf(void);
void set_cmd_rf(void);
void output_string_serial_rf(char const *string);

#endif //SERIAL_RF_H
