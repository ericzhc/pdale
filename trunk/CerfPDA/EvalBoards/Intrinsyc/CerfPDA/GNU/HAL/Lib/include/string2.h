//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001 Intrinsyc Software Inc. All rights reserved.
//
// Module name:
//
//      string.h
//
// Description:
//
//      Various string manipulation functions.
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

#ifndef STRING_H
#define STRING_H

#include <types.h>

int itoa(int number, char *buf);
void u32toa(u32 number, char *buf);
u32 atoip(char const *ip);
char const * iptoa(u32 addr);
int atoversion(char const *version, u8 *major, u8 *minor, u8 *build, u8 *relinfo);
void u8toa(u8 number, char *result);
void u16toa(u16 number, char *result);
void chopstr(char *string);
char const *next_token(char const *string);

#endif
