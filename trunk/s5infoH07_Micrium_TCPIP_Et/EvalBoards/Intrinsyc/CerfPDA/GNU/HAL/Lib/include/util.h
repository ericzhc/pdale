//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001 Intrinsyc Software Inc. All rights reserved.
//
// Module name:
//
//      util.h
//
// Description:
//
//      Various stdlib like utility functions.
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

#ifndef UTIL_H
#define UTIL_H

// Number of elements in an array
#define countof(a) (sizeof(a) / sizeof((a)[0]))

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

void print_bytes(u8 const *start, int size);
void print_words(u16 const *start, int size);
void print_dwords(u32 const *start, int size);

#endif
