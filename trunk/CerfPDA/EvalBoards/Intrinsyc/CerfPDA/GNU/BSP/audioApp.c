/**
 *********************************************************************
 *  @file       audioApp.c
 *  @version    0.1
 *  @author     ELR
 *  @date       20 Mars 2007 
 *
 *  Project:    ---
 *
 *  @brief      CerfPDA audio driver demo
 *
 *  @todo 
 *
 *
 *
 *  Revision History:
 * 
 * 
 *
 *  COPYRIGHT (C) 2007 Eduardo Luis Romero. Université de Sherbrooke.
 *  Québec, Canada.
 *
 *********************************************************************/


/* //////////////////////////////////////////////////////////////////////////// */
/* include files                           //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */


#include <includes.h>
#include <app_cfg.h>
#include <audioDrv.h>
#include <erG711.h>
#include <debugELR2.h>
#include <string.h>
#include <audiobufferdefinition.h>

/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */



/* //////////////////////////////////////////////////////////////////////////// */
/* extern variables                         //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

extern char    _bss;
extern char    _ebss;

/* //////////////////////////////////////////////////////////////////////////// */
/* global variables                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */


/* //////////////////////////////////////////////////////////////////////////// */
/* local variables                         //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */



enum {
    A_STOP = 0,
    A_PLAY,
    A_REC
};

INT8U       *audioBufP;
INT16U       frameSize;
INT16U       recFlg = 0;
INT16U       audioMode = A_STOP;

/* //////////////////////////////////////////////////////////////////////////// */
/* local tables                            //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */


INT8U       audioBuffer[AUDIOBUFFSIZE] ;


										


/* //////////////////////////////////////////////////////////////////////////// */
/* local function prototypes               //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

void        startRec            (void);
void        startPlay           (void);
void        tstAudioPCM_rec     (void);
void        tstAudioPCM_play    (void);
void        menuFn              (char dByte);

/* //////////////////////////////////////////////////////////////////////////// */
/* global functions                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */


void tstAudioPCM_play(void)
{
    static INT16U   cnt = 0;
    INT16S         *dP;

    if( audioMode == A_PLAY ){  
        dP =(INT16S*) outChnnlNxtBuf();
        erG711A_decoder(audioBufP,dP,frameSize);
        audioBufP += frameSize;
        if( audioBufP >= &audioBuffer[AUDIOBUFFSIZE] ){
            audioMode = A_STOP;
            audioIoChnnlStop();
            erD_sndstr("\n\rPlay_END\n\r");
        }
        else if(!(++cnt & 0x03))
                erD_sndchr('#');
    }
}

void tstAudioPCM_rec(void)
{
    static   INT16U  cnt = 0;
    INT16S          *dP;

    if( audioMode == A_REC ){
        dP =(INT16S*) inChnnlNxtBuf();
        erG711A_encoder(dP,audioBufP,frameSize);
        audioBufP += frameSize;
        if( audioBufP >= &audioBuffer[AUDIOBUFFSIZE] ){
            audioMode = A_STOP;
            audioIoChnnlStop();
            erD_sndstr("\n\rRec_END\n\r");
            recFlg = 1;
            startPlay();
        }
        else if(!(++cnt & 0x03))
                erD_sndchr('@');
    }
}


void startRec(void)
{

    erD_sndstr("\n\rRec Setup\n\r");
    erD_sndstr("Format: PCM A\n\r");
    if(audioMode != A_STOP){
        erD_sndstr("Error: audio machine is running\n\r");
        return;        
    }
    audioMode = A_REC;
    frameSize = 160;
    audioBufP = &audioBuffer;
    inChnnlInit (AUDIO_FSAMP_8000, frameSize, tstAudioPCM_rec);
    outChnnlInit(AUDIO_FSAMP_8000, frameSize, tstAudioPCM_play);
    audioIoChnnlStart();
    erD_sndstr("Recording...\n\r");
}


void startPlay(void)
{

    erD_sndstr("\n\rPlay Setup\n\r");
    erD_sndstr("Format: PCM A\n\r");
    /*
	if( !recFlg ){
        erD_sndstr("Error: NO Recorded audio is available\n\r");
        return;
    }*/
    if(audioMode != A_STOP){
        erD_sndstr("Error: audio machine is running\n\r");
        return;        
    }
    audioMode = A_PLAY;
    
    frameSize = 160;
    audioBufP = &audioBuffer;
    inChnnlInit (AUDIO_FSAMP_8000, frameSize, tstAudioPCM_rec);
    outChnnlInit(AUDIO_FSAMP_8000, frameSize, tstAudioPCM_play);
    audioIoChnnlStart();
    erD_sndstr("Playing...\n\r");
}



