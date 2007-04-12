/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                               (c) Copyright 2004, Micrium, Inc., Weston, FL
*                                           All Rights Reserved
*
*                                            GNU Linker Script
*
* File    : ldscript
* Modif by: Jean-Denis Hatier
*
*********************************************************************************************************
*/

OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf32-littlearm")
ENTRY(main)

SECTIONS
{
   . = 0x20100000;

   . = ALIGN(4);
   .boot    : { *(.boot)   }

   . = ALIGN(4);
   .text    : { *(.text)   }

   . = ALIGN(4);
   .rodata  : { *(.rodata) }

   . = ALIGN(4);
   .data    : { *(.data)   }

   . = ALIGN(4);
   _bss     = .;
   .bss     : { *(.bss)    }
   _ebss    = .;
   end      = .;

   . = ALIGN(4);
   _estack  = .;
   _heap    = .;
   .        = . + 0x800000;  /* Global stack and heap memory size: 8Mb */
   _stack   = .;
   _eheap   = .;
}
