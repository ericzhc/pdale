//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001 Intrinsyc Software Inc. All rights reserved.
//
// Module name:
//
//      serial.h
//
// Description:
//
//      Interfaces with the StrongArm SA1110 serial hardware.
//
// Author:
//
//      Mike Kirkland
//
// Created:
//
//      October 2001
//
////////////////////////////////////////////////////////////////////////////////

#ifndef SERIAL_FRONT_H
#define SERIAL_FRONT_H

#include <types.h>

void init_serial_front(u32 baud);
void output_byte_serial_front(char byte);
int input_byte_serial_front(char *byte);

#endif //SERIAL_H
