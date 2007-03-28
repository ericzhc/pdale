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
ENTRY(_start)

SECTIONS
{
   . = 0xC0000000;

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
   .        = . + 0x2000000;  /* Global stack and heap memory size: 32Mb */
   _stack   = .;
   _eheap   = .;
}

int_vector    = 0xc3ef0000;
frame_buffer  = 0xc3f00000;
mmu_tbl       = 0xc3f40000;
