#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
//#include <../../../uC-GUI/Ex1/includes.h>

#define F1 0x01
#define F2 0x02
#define SHIFT 0x16
#define ESCAPE 0x1b
#define ENTER 0x15
#define CAPSCTRL 0x04
#define NUMCUR 0x05
#define UP 0x11	
#define DOWN 0x12
#define LEFT 0x13
#define RIGHT 0x14
#define POWER 0x03
#define BACKSPACE 0x08
#define TAB 0x09
#define SPACE 0x20
#define MAX_LINE_SIZE 64

#define CERF_PDA_CPLD_BASE_ADD          0x40000000
#define CERF_PDA_CPLD_KEYB_SCNLOW       *(char*)(CERF_PDA_CPLD_BASE_ADD + 0x06)    /* Keyboard scan out low nibble         */
#define CERF_PDA_CPLD_KEYB_SCNHIGH      *(char*)(CERF_PDA_CPLD_BASE_ADD + 0x0a)    /* Keyboard scan out high nibble        */
#define KEYBLINE						*(volatile int*)(0x90040000)    /* Sound left-righ reset (sync)         */

#define GPLR_SF *(volatile unsigned char*) (0x90040000)
#define GPDR_SF *(volatile unsigned char*) (0x90040000 + 0x04)
#define GAFR_SF *(volatile unsigned char*) (0x90040000 + 0x1C)
#define GFER_SF *(volatile unsigned char*) (0x90040000 + 0x14)
#define GRER_SF *(volatile unsigned char*) (0x90040000 + 0x10)
#define GEDR_SF *(volatile unsigned char*) (0x90040000 + 0x18)
#define GPCR_SF *(volatile unsigned char*) (0x90040000 + 0x0C)

#define SER3_UTCR0     (*(volatile int *)  0x80010000) 
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

char *curmap;

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
char  ReadFromKeyboard(void);
char scanColumn(int columb);
char getkeyNumber(int key);
char getKey(int key, int columb);
void ReadLine(char * line, int Max_Line_Size);

#endif
