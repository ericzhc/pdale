//////////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2001 Intrinsyc Software Inc. All rights reserved.
//
// Module name:
//
//      string.c
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

#include <stdarg.h>
#include <types.h>
#include <serial.h>
#include <util.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// itoa
// PURPOSE: Converts an int into its ASCII decimal representation.
// PARAMS:  (IN)  int number - number to convert.
//          (OUT) char *buf  - buffer to write string into.
// RETURNS: int - number of characters written into buf.
// NOTES:   Does NOT null terminate the string, to allow for replacing %I and
//          trailing spaces with the string in itc_printf.
//          Does NOT work when number == INT_MIN
////////////////////////////////////////////////////////////////////////////////
int
itoa(int number,
     char *buf)
{
   int divisor;
   int digit = 0;
   int started = 0;
   int i = 0;

   if (number < 0)
   {
      // Convert negative number to positive to display
      // Note that this will fail for the most negative possible integer
      // because that one number doesn't have a positive equivalent.
      *buf++ = '-';
      ++i;
      number = -number;
   }

   for(divisor = 1000000000; divisor > 0; divisor /= 10)
   {
      digit = number / divisor;
      number %= divisor;

      if(digit >= 0 && digit < 10)
      {
         if(started || digit > 0)
         {
            *buf++ = (char)('0' + digit);
            started = 1;
            i++;
         }
      }
   }

   if(!started)
   {
      *buf = '0';
      return 1;
   }

   return i;
}

////////////////////////////////////////////////////////////////////////////////
// u32toa
// PURPOSE: Converts a u32 into its ASCII hexadecimal representation.
// PARAMS:  (IN)  u32 number - number to convert.
//          (OUT) char *buf  - buffer to write string into.
// RETURNS: Nothing. (Number of characters is always 8.)
// NOTES:   Does NOT null terminate the string, to allow for replacing %X and
//          trailing spaces with the string in itc_printf.
////////////////////////////////////////////////////////////////////////////////
void
u32toa(u32 number,
       char *buf)
{
   int i;

   for(i = (sizeof(u32) * 2) - 1;i >= 0;i--)
   {
      u32 temp = (number >> (i * 4)) & 0x0F;
      *buf++ = temp + ((temp <= 9) ? '0' : 'A' - 10);
   }
}

////////////////////////////////////////////////////////////////////////////////
// atoip
// PURPOSE: Converts an ASCII IP address (e.g. 192.168.1.1) to a u32.
// PARAMS:  (IN)  char *ip - IP address to convert.
// RETURNS: IP address as a u32.
////////////////////////////////////////////////////////////////////////////////
u32
atoip(char const *ip)
{
   u32 result = 0;
   u32 temp = 0;
   int i = 0;

   do
   {
      if(*ip >= '0' && *ip <= '9')
      {
         temp *= 10;
         temp += *ip - '0';
      }
      if(*ip == '.' || *ip == 0 || *ip == ' ')
      {
	 if(temp > 255)
	 {
	    return 0;
	 }
         i++;
         result <<= 8;
         result |= temp;
         temp = 0;
      }
   } while(*ip != 0 && *ip++ != ' ');

   if(i == 4)
   {
      return result;
   }
   else
   {
      return 0;
   }
}

////////////////////////////////////////////////////////////////////////////////
// iptoa
// PURPOSE: Returns a static string with an IP address in dotted decimal
// PARAMS:  (IN) u32 addr - IP address
// RETURNS: pointer to static string
////////////////////////////////////////////////////////////////////////////////
char const *
iptoa(u32 addr)
{
   static char ipstring[17];

   // itoa doesn't null terminate the strings so do it now
   memset8(ipstring, 0, sizeof(ipstring));
   itoa((u8)((addr & 0xFF000000) >> 24), ipstring);
   ipstring[itc_strlen(ipstring)] = '.';
   itoa((u8)((addr & 0x00FF0000) >> 16), ipstring+itc_strlen(ipstring));
   ipstring[itc_strlen(ipstring)] = '.';
   itoa((u8)((addr & 0x0000FF00) >> 8), ipstring+itc_strlen(ipstring));
   ipstring[itc_strlen(ipstring)] = '.';
   itoa((u8)(addr), ipstring+itc_strlen(ipstring));
   return ipstring;
}

////////////////////////////////////////////////////////////////////////////////
// atoversion
// PURPOSE: Converts an ASCII dotted version quartet into 4 u8s.
// PARAMS:  (IN)  char *version - ASCII dotted quartet.
//          (OUT) u8 *major     - First number.
//          (OUT) u8 *minor     - Second number.
//          (OUT) u8 *build     - Third number.
//          (OUT) u8 *relinfo   - Fourth number.
// RETURNS: 1 for success, 0 for failure.
////////////////////////////////////////////////////////////////////////////////
int
atoversion(char const *version,
           u8 *major,
           u8 *minor,
           u8 *build,
           u8 *relinfo)
{
   u8 *current;
   int i = 0;

   current = major;
   *current = 0;

   do
   {
      if(*version >= '0' && *version <= '9')
      {
         *current *= 10;
         *current += *version - '0';
      }
      if(*version == '.')
      {
         if(current == major)
	 {
            current = minor;
	 }
         else if(current == minor)
	 {
            current = build;
	 }
         else if(current == build)
	 {
            current = relinfo;
	 }
         *current = 0;
         i++;
      }
   } while(*version++ != 0);

   return (i < 3 ? 0 : 1);
}

////////////////////////////////////////////////////////////////////////////////
// u8toa
// PURPOSE: Converts a u8 into its ASCII hexadecimal .
// PARAMS:  (IN)  u8 number    - number to convert.
//          (OUT) char *result - buffer to write ASCII to.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
u8toa(u8 number,
      char *result)
{
   int digit, i;

   for(i = 4; i >= 0; i -= 4)
   {
      digit = (((0x0F << i) & number) >> i);
      if(digit < 10)
      {
         *result++ = '0' + digit;
      }
      else
      {
         *result++ = 'A' + digit - 10;
      }
   }
   *result = 0;
}

////////////////////////////////////////////////////////////////////////////////
// u16toa
// PURPOSE: Converts a u16 into its ASCII hexadecimal .
// PARAMS:  (IN)  u16 number   - number to convert.
//          (OUT) char *result - buffer to write ASCII to.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
u16toa(u16 number,
       char *result)
{
   int digit, i;

   for(i = 12; i >= 0; i -= 4)
   {
      digit = (((0x000F << i) & number) >> i);
      if(digit < 10)
      {
         *result++ = '0' + digit;
      }
      else
      {
         *result++ = 'A' + digit - 10;
      }
   }
   *result = 0;
}

////////////////////////////////////////////////////////////////////////////////
// chopstr
// PURPOSE: Converts a string ending in a '\r' or '\n' into a null terminated
//          string.
// PARAMS:  (IN/OUT) char *string - string to chop.
// RETURNS: Nothing.
////////////////////////////////////////////////////////////////////////////////
void
chopstr(char *string)
{
   while(*string != 0)
   {
      if(*string == '\r' || *string == '\n')
      {
         *string = 0;
         break;
      }
      string++;
   }
}

////////////////////////////////////////////////////////////////////////////////
// next_token
// PURPOSE: Skips to the first character of the next token.
// PARAMS:  char *string - string to parse.
// RETURNS: char * - pointer to offset in string.
////////////////////////////////////////////////////////////////////////////////
char const
*next_token(char const *string)
{
   if(*string == ':')
   {
      string++;
   }

   //skip to the next whitespace, skipping over backslash escaped characters
   while(*string != 0 && !(*string == ' ' || *string == ':'))
   {
      if (*string == '\\' && *(string+1))
      {
         string++;
      }
      string++;
   }

   //and pass it over
   while(*string != 0 && *string == ' ')
   {
      string++;
   }

   return string;
}
