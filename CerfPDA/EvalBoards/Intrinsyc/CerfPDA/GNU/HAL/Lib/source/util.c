//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001 Intrinsyc Software Inc. All rights reserved.
//
// Module name:
//
//      util.c
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

#include <types.h>
#include <string.h>
#include <util.h>

////////////////////////////////////////////////////////////////////////////////
// print_bytes
// PURPOSE: Prints the values of size bytes.
// PARAMS:  (IN) u8 *start - starting address.
//          (IN) int size  - number of bytes to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
print_bytes(u8 const *start,
            int size)
{
   int x = 1;

   while(size > 0)
   {
      itc_printf("%b", *start++);
      size--;
      if(!(x++ % 27))
      {
         if(size > 0)
         {
            itc_printf("\r\n");
            x = 1;
         }
      }
      else
      {
         itc_printf(" ");
      }
   }
   itc_printf("\r\n");
}

////////////////////////////////////////////////////////////////////////////////
// print_words
// PURPOSE: Prints the values of size words.
// PARAMS:  (IN) u8 *start - starting address.
//          (IN) int size  - number of words to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
print_words(u16 const *start,
            int size)
{
   int x = 1;

   while(size > 0)
   {
      char buf[5];

      u16toa(*start++, buf);
      itc_printf(buf);
      size--;
      if(!(x++ % 16))
      {
         if(size > 0)
         {
            itc_printf("\r\n");
            x = 1;
         }
      }
      else
      {
         itc_printf(" ");
      }
   }
   itc_printf("\r\n");
}

////////////////////////////////////////////////////////////////////////////////
// print_dwords
// PURPOSE: Prints the values of size dwords.
// PARAMS:  (IN) u8 *start - starting address.
//          (IN) int size  - number of dwords to print.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
print_dwords(u32 const *start,
            int size)
{
   int x = 1;

   while(size > 0)
   {
      itc_printf("%x", *start++);
      size--;
      if(!(x++ % 8))
      {
         if(size > 0)
         {
            itc_printf("\r\n");
            x = 1;
         }
      }
      else
      {
         itc_printf(" ");
      }
   }
   itc_printf("\r\n");
}
