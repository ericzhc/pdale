/**********************************************************************
 *	$Workfile:   SMA_types.h  $
 *	$Revision:   1.14  $
 *	$Author:   kovitzp  $
 *	$Date:   01 Apr 2002 09:29:46  $
 *
 *	Project: Common Include Files
 *
 *	Description:
 *      SMA_types.h contains the Sharp SMA typedefs for C standard types.
 *      It is intended to be used in ISO C conforming development 
 *      environments and checks for this insofar as it is possible
 *      to do so.
 *
 *
 *      SMA_types.h ensures that the name used to define types correctly
 *      identifies a representation size, and by direct inference the
 *      storage size, in bits. E.g., UNS_32 identifies an unsigned
 *      integer type stored in 32 bits.
 *
 *      It requires that the basic storage unit (char) be stored in 
 *      8 bits.
 *
 *      No assumptions about Endianess are made or implied.
 *
 *      SMA_types.h also contains Sharp SMA Global Macros:
 *          _BIT
 *          _SBF
 *          _BITMAP
 *      These #defines are not strictly types, but rather Preprocessor
 *      Macros that have been found to be generally useful.
 *
 *  Dependencies:
 *  	ISO C library include files:
 *  		limits.h
 *  		float.h
 *
 *	Revision History:
 *	$Log:   P:/PVCS6_6/archives/SOC/include/SMA_types.h-arc  $
 * 
 *    Rev 1.14   01 Apr 2002 09:29:46   kovitzp
 * Added legal disclaimer.
 * 
 *    Rev 1.13   Feb 06 2002 17:12:02   BarnettH
 * Added omitted semicolon
 * 
 *    Rev 1.12   Feb 06 2002 11:57:42   BarnettH
 * Created three char ranked types: CHAR, UNS_8, INT_8 and removed (CHAR_MIN == SCHAR_MIN) conditional on last two types.
 * 
 *    Rev 1.11   Feb 05 2002 19:09:12   BarnettH
 * Added CHAR_MIN == SCHAR_MIN compare to determine UNS_8 and INT_8 typedefs.
 * 
 *    Rev 1.10   Jan 29 2002 11:23:30   KovitzP
 * Removed BOOL type. Replaced it with BOOL_32, BOOL_16,
 * and BOOL_8.
 * 
 *    Rev 1.9   Jan 28 2002 18:06:38   KovitzP
 * Added BOOL type.
 * 
 *    Rev 1.8   Jan 09 2002 15:51:16   BarnettH
 * Changed FP_64 to depend on DBL_MAX_EXP value
 * 
 *    Rev 1.7   Jan 04 2002 09:56:36   BarnettH
 * Added comment for _BITMASK
 * Added _ERROR, _NO_ERROR macros
 * 
 *    Rev 1.6   Nov 13 2001 10:31:18   KovitzP
 * added 64-bit types.
 * 
 *    Rev 1.5   Sep 20 2001 19:55:44   BarnettH
 * Added _BIT, _SBF, _BITMASK definitions
 * 
 *    Rev 1.4   Sep 12 2001 11:35:06   KovitzP
 * Add floating point type definitions.
 * 
 *    Rev 1.3   Aug 30 2001 14:40:32   BarnettH
 * Removed uC/OS-II typedefs.
 * 
 *    Rev 1.2   Aug 29 2001 14:25:30   KovitzP
 * Made microC/OS-II type definitions dependent on the predefinition of SMA_TYPES_USE_UCOSII
 * 
 *    Rev 1.1   Aug 28 2001 16:57:48   KovitzP
 * changed SMA_types so that if MicroC/OS-II has already defined some of the types, then there will be no type conflicts.
 * 
 *    Rev 1.0   Jul 16 2001 15:46:20   KovitzP
 * Initial revision.
 * 
 * SHARP MICROELECTRONICS OF THE AMERICAS MAKES NO REPRESENTATION
 * OR WARRANTIES WITH RESPECT TO THE PERFORMANCE OF THIS SOFTWARE,
 * AND SPECIFICALLY DISCLAIMS ANY RESPONSIBILITY FOR ANY DAMAGES, 
 * SPECIAL OR CONSEQUENTIAL, CONNECTED WITH THE USE OF THIS SOFTWARE.
 *
 * SHARP MICROELECTRONICS OF THE AMERICAS PROVIDES THIS SOFTWARE SOLELY 
 * FOR THE PURPOSE OF SOFTWARE DEVELOPMENT INCORPORATING THE USE OF A 
 * SHARP MICROCONTROLLER OR SYSTEM-ON-CHIP PRODUCT. USE OF THIS SOURCE
 * FILE IMPLIES ACCEPTANCE OF THESE CONDITIONS.
 *
 *	COPYRIGHT (C) 2001 SHARP MICROELECTRONICS OF THE AMERICAS, INC.
 *		CAMAS, WA
 *********************************************************************/

#ifndef SMA_TYPES_H
#define SMA_TYPES_H

#include <limits.h>
#include <float.h>

#ifndef __STDC__ 
#error ERROR: Environment not ISO C conforming.
#endif

#if CHAR_BIT != 8
#error Basic storage unit size must be defined as 8 bits in <limits.h>
#endif

/* Unconditional typedef */ 
typedef char            CHAR;

#if UCHAR_MAX == 255
typedef unsigned char	UNS_8;
#endif /* end UCHAR_MAX */ 

#if SCHAR_MAX == 127
typedef signed char		INT_8;
#endif /* end SCHAR_MAX */ 

#if USHRT_MAX == 65535
typedef	unsigned short	UNS_16;
#endif

#if SHRT_MAX == 32767
typedef	signed short	INT_16;
#endif

#if UINT_MAX == 0xFFFFFFFFU
typedef	unsigned int	UNS_32;
#elif ULONG_MAX == 0xFFFFFFFFU
typedef	unsigned long	UNS_32;
#endif

#if INT_MAX == 0x7FFFFFFF
typedef	signed int		INT_32;
#elif LONG_MAX == 0x7FFFFFFF
typedef	signed long		INT_32;
#endif

#if LLONG_MAX == 0x7fffffffffffffffLL
typedef long long INT_64;
#endif

#if ULLONG_MAX == 0xffffffffffffffffULL
typedef unsigned long long UNS_64;
#endif

#if 0
#if FLT_MANT_DIG == 24 && FLT_MAX_EXP == 128
typedef float          FP_32;	/* Single precision floating point */
#endif
#if DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024
typedef double         FP_64;	/* Double precision floating point */
#endif
#endif

/********************************************************************
 * Global Macros
 *******************************************************************/

/* _BIT(n) sets the bit at position "n"
 * _BIT(n) is intended to be used in "OR" and "AND" expressions:
 * e.g., "(_BIT(3) | _BIT(7))".
 */
#undef _BIT
#define _BIT(n)	(((UNS_32)(1)) << (n))

/* _SBF(f,v) sets the bit field starting at position "f" to value "v".
 * _SBF(f,v) is intended to be used in "OR" and "AND" expressions:
 * e.g., "((_SBF(5,7) | _SBF(12,0xF)) & 0xFFFF)"
 */ 
#undef _SBF
#define _SBF(f,v) (((UNS_32)(v)) << (f))

/* _BITMASK constructs a symbol with 'field_width' least significant
 * bits set.
 * e.g., _BITMASK(5) constructs '0x1F', _BITMASK(16) == 0xFFFF
 * The symbol is intended to be used to limit the bit field width
 * thusly:
 * <a_register> = (any_expression) & _BITMASK(x), where 0 < x <= 32.
 * If "any_expression" results in a value that is larger than can be
 * contained in 'x' bits, the bits above 'x - 1' are masked off.  When
 * used with the _SBF example above, the example would be written:
 * a_reg = ((_SBF(5,7) | _SBF(12,0xF)) & _BITMASK(16))
 * This ensures that the value written to a_reg is no wider than 
 * 16 bits, and makes the code easier to read and understand.
 */ 
#undef _BITMASK
#define _BITMASK(field_width) ( _BIT(field_width) - 1)

/* Generic "error" definitions */ 
#define _NO_ERROR   0
#define _ERROR      -1

/* Generic BOOL types */
typedef INT_32 BOOL_32;
typedef INT_16 BOOL_16;
typedef INT_8 BOOL_8;
#if !defined FALSE
#define FALSE (0==1)
#endif
#if !defined TRUE
#define TRUE (!(FALSE))
#endif
#endif /* SMA_TYPES_H */ 
