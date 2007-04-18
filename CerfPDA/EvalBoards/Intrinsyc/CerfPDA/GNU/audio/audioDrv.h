/**
 *********************************************************************
 *	@file       erAudioDev.h
 *	@version    0.1
 *	@author     ELR
 *	@date       24 Avril 2006 
 *
 *	Project:    Sip Stack
 *
 *  @brief      low level CerfPDA's audio driver
 *
 *  @todo 
 *
 *
 *	Revision History:
 * 
 * 
 *
 *	COPYRIGHT (C) 2006 Eduardo Luis Romero. Université de Sherbrooke.
 *  Québec, Canada.
 *
 *********************************************************************/
#ifndef _ERAUDIO_H_
#define _ERAUDIO_H_

/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */
#define L3_DATA		    0x0080                                                      /* L3 bus: data line                            */
#define L3_CLK		    0x0100                                                      /* L3 bus: clock                                */
#define L3_MODE		    0x0200                                                      /* L3 bus: mode                                 */
#define L3_DELAY	    2
#define ADDR_MODE       0
#define FIRST_BYTE      1

#define PPAR_ADDR	  *(volatile u32*) 0x90060008                                   /* Pin Assignment Register                      */

#define SOUNDDEVBUFF_MAXSIZE    200                                                 /* max sound dev. buffer size [samples]         */


/* Sampling frecuency (256x)        */
/* See CPLD register at offset 0x04 */
typedef enum cPDAfrSampl_e {
            AUDIO_FSAMP_4000,
            AUDIO_FSAMP_8000,
            AUDIO_FSAMP_16000,
            AUDIO_FSAMP_32000,
            AUDIO_FSAMP_5512_5,
            AUDIO_FSAMP_11025,
            AUDIO_FSAMP_22050,
            AUDIO_FSAMP_44100,
            AUDIO_FSAMP_6000,
            AUDIO_FSAMP_12000,
            AUDIO_FSAMP_24000,
            AUDIO_FSAMP_48000,
            AUDIO_FSAMP_96000
            }cPDAfrSampl_e;

#define DEFAULT_AUDIO_FSAMP AUDIO_FSAMP_8000

/* //////////////////////////////////////////////////////////////////////////// */
/* function prototypes                     //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */
void   iniAudioDvce(void);

void   inChnnlInit(cPDAfrSampl_e samplRate,INT16U bufSize, void (*onBufDone)(void));
void   inChnnlClrBuf(void);
INT16S inChnnlStart(void);
INT16S inChnnlStop(void);
void   inChnnlReset(void);
void*  inChnnlNxtBuf(void);
void*  inChnnlBufInUse(void);

void   outChnnlInit(cPDAfrSampl_e samplRate,INT16U bufSize, void (*onBufDone)(void));
void   outChnnlClrBuf(void);
INT16S audioIoChnnlStart(void);
INT16S audioIoChnnlStop(void);
INT16S audioIoChnnlClose(void);
INT16S outChnnlStart(void);
INT16S outChnnlStop(void);
void   outChnnlReset(void);
void*  outChnnlNxtBuf(void);
void*  outChnnlBufInUse(void);
void   iniSampleTime( INT32U iniVal);
INT32U getSampleTime(void);
INT32U getCurrentSTime(void);

#endif
