/*
*********************************************************************************************************
*   
*   
*
*   
*   
*
*
----------------------------------------------------------------------
File        : debug.h
Purpose     : aux debug tools API include file
---------------------------END-OF-HEADER------------------------------
*/

#ifndef  KEYPAD_H
#define  KEYPAD_H


/*
*********************************************************************************************************
*                                               AUX-DEBUG API
*********************************************************************************************************
*/

void erD_sndchr(unsigned char chr);
void erD_sndchrRep(unsigned char chr, unsigned short rep);
void erD_sndstr(const unsigned char *string);
void erD_snd_cr(void);
void erD_sndValByte(unsigned char val);
void erD_sndValByteLbl(char* label, unsigned char val);
void erD_sndValHWrd(unsigned short val);
void erD_sndValHWrdLbl(char* label, unsigned short val);
void erD_sndValWrd(unsigned int val);
void erD_sndValWrdLbl(char* label, unsigned int val);
void erD_byteDump(unsigned char *valP, unsigned short cnt);
void erD_hwDump(unsigned short *valP, unsigned short cnt);
void erD_Trace_1(void);
void erD_Trace_2(void);
void erD_Trace_3(void);
void erD_Trace_4(void);
void erD_Trace_5(void);
void erD_Trace_6(void);
void erD_Trace_7(void);
void erD_Trace_8(void);
void erD_Trace_9(void);

#endif   /* ifdef KEYPAD_H */
