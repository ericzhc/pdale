
/*
*********************************************************************************************************
*                                               CerfPDA
*                                           AUX DEBUG TOOLS
* File    : debug.c
* By      : Eduardo Luis Romero  22/11/2005
*
*********************************************************************************************************
*/

//#include <includes.h>
#include <serial.h>
#include "debugELR2.h"


/*
*********************************************************************************************************
*                                               CONSTANTS & MACROS
*********************************************************************************************************
*/
#define ELRSNDCHAR(a)   output_byte_serial(a)

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void erD_dumpAdd( unsigned valP);

/*
*********************************************************************************************************
*                                             C ENTRY POINT
*********************************************************************************************************
*/

const static unsigned char toHex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void erD_sndchr(unsigned char chr)
{
    ELRSNDCHAR(chr);
}

void erD_sndchrRep(unsigned char chr, unsigned short rep)
{
    while(rep--)
        ELRSNDCHAR(chr);
}

void erD_sndstr(const unsigned char *string)
{
    // Note: string max size: 40 chars
    unsigned short  maxSize = 40;

    while(maxSize--) {
        if (*string == 0)
            break;
        ELRSNDCHAR(*string++);
   }
}

void erD_snd_cr(void)
{
    ELRSNDCHAR('\r');
    ELRSNDCHAR('\n');
}

// send byte as 2 hex chars
void erD_sndValByteD(unsigned char val)
{
   ELRSNDCHAR (toHex[(val >> 4)& 0x0f]);
   ELRSNDCHAR (toHex[val & 0x0f]);
}
// send byte as 2 hex chars with separator (' ')
void erD_sndValByte(unsigned char val)
{
   ELRSNDCHAR(' ');
   erD_sndValByteD(val);
}

// send byte as 2 hex chars with separator (' ') & Label
void erD_sndValByteLbl(char* label, unsigned char val)
{
   ELRSNDCHAR(' ');
   erD_sndstr(label);
   ELRSNDCHAR(':');
   erD_sndValByte(val);
}

// send a half-word (16 bits) as 4 hex chars with separator
void erD_sndValHWrd(unsigned short val)
{
    ELRSNDCHAR(' ');
    erD_sndValByteD((unsigned char)(val>>8));
    erD_sndValByteD((unsigned char)(val&0xff));
}

// send a half-word (16 bits) as 4 hex chars with separator & label
void erD_sndValHWrdLbl(char* label, unsigned short val)
{
    ELRSNDCHAR(' ');
    erD_sndstr(label);
    ELRSNDCHAR(':');
    erD_sndValHWrd(val);
}

// send a word (32 bits) as 8 hex chars with separator
void erD_sndValWrd(unsigned int val)
{
    ELRSNDCHAR(' ');
    erD_sndValByteD((unsigned char)((val/0x1000000) & 0xff));
    erD_sndValByteD((unsigned char)((val/0x10000) & 0xff));
    erD_sndValByteD((unsigned char)((val/256) & 0xff));
    erD_sndValByteD((unsigned char)(val & 0xff));

}

// send a word (32 bits) as 8 hex chars with separator & label
void erD_sndValWrdLbl(char* label, unsigned int val)
{
    ELRSNDCHAR(' ');
    erD_sndstr(label);
    ELRSNDCHAR(':');
    erD_sndValWrd(val);
}

void erD_byteDump(unsigned char *valP, unsigned short cnt)
{
    unsigned short lcnt;
    unsigned char  *valPch;
    unsigned       align;

    erD_dumpAdd((unsigned)valP & ~0x0f);
    align = (unsigned)valP & 0x0f;
    while( align--){
        erD_sndchr(' ');
        erD_sndchrRep('-',2);
    }
    align = (unsigned)valP & 0x0f;
    lcnt = 16 - align;
    valPch = valP;
    while(cnt--){
        erD_sndValByte(*valP++);
        if(!--lcnt){
            erD_sndstr("   ");
            for(lcnt = 16; lcnt>0; lcnt--){
                if(align){
                    align--;
                    erD_sndchr(' ');
                }
                else {
                    if(*valPch < ' ' || *valPch > '~')
                        erD_sndchr('.');
                    else
                        erD_sndchr(*valPch);
                    valPch++;
                }
            }
            erD_dumpAdd((unsigned)valP);
            lcnt = 16;
        }
    }
    erD_snd_cr();
}

void erD_hwDump(unsigned short *valP, unsigned short cnt)
{
    unsigned short lcnt;

    if((unsigned)valP & 1)
        valP = (unsigned short *)((unsigned)valP & ~0x01);
    erD_dumpAdd((unsigned)valP & ~0x07);
    lcnt = (unsigned)valP & 0x07;
    while( lcnt--){
        erD_sndchrRep('-',4);
        erD_sndchr(' ');
    }
    lcnt = 8;
    while(cnt--){
        erD_sndValHWrd(*valP++);
        if(!--lcnt){
            erD_dumpAdd((unsigned)valP);
            lcnt = 8;
        }
    }
    erD_snd_cr();
}

void erD_dumpAdd( unsigned valP)
{
    erD_snd_cr();
    erD_sndValWrd(valP);
    ELRSNDCHAR(':');
    ELRSNDCHAR(' ');
}

void erD_Trace_1(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('1');
    ELRSNDCHAR(']');
}

void erD_Trace_2(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('2');
    ELRSNDCHAR(']');
}

void erD_Trace_3(void)
{
    ELRSNDCHAR('[');    
    ELRSNDCHAR('3');
    ELRSNDCHAR(']');
}

void erD_Trace_4(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('4');
    ELRSNDCHAR(']');
}

void erD_Trace_5(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('5');
    ELRSNDCHAR(']');
}

void erD_Trace_6(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('6');
    ELRSNDCHAR(']');
}

void erD_Trace_7(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('7');
    ELRSNDCHAR(']');
}

void erD_Trace_8(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('8');
    ELRSNDCHAR(']');
}

void erD_Trace_9(void)
{
    ELRSNDCHAR('[');
    ELRSNDCHAR('9');
    ELRSNDCHAR(']');
}

