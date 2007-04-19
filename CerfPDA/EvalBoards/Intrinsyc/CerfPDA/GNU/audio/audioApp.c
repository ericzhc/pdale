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


OS_STK  startTaskStk[START_TASK_STK_SIZE];


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

OS_STK      startTaskStk[START_TASK_STK_SIZE];
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
	delay = 1;
    memset (&_bss, 0, &_ebss-&_bss);

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



void menuFn(char dByte)
{
    void        (*restart_vector)(void) = 0;

    switch(dByte){
		case '\\':
			restart_vector = NULL;
            printf("Restarting...");
            udelay(3000);
            restart_vector();
            break;
        case 'R':
        case 'r':
            startRec();
            break;
        case 'P':
        case 'p':
            startPlay();
            break;
        case 'E':
        case 'e':
            if(audioMode != A_STOP)
                erD_sndstr("Error: audio machine is running\n\r");
            else{
                memset(audioBuffer, 0, sizeof(audioBuffer));
                recFlg = 0;
            }
            break;
        case 'S':
        case 's':
            audioMode = A_STOP;
            audioIoChnnlStop();
            erD_sndstr("\n\rSTOP\n\r");
            break;
        case 'M':
        case 'm':
            erD_hwDump((void*)audioBuffer+512 , 512);
            break;
        case 'X':
        case 'x':
            erD_hwDump((void*)inChnnlNxtBuf() , 128);
            erD_hwDump((void*)inChnnlBufInUse() , 128);
            break;
        case 'Y':
        case 'y':
            erD_hwDump((void*)outChnnlNxtBuf() , 128);
            erD_hwDump((void*)outChnnlBufInUse() , 128);
            break;
        case 'I':
        case 'i':
            erD_sndstr("\n\rS5 info\n\r");
            erD_sndstr("\n\rCerfPDA audio demo\n\r");
            break;
        case 'H':
        case 'h':
            erD_sndstr("\n\n\rHELP\n\r");
            erD_sndstr("R: record\n\r");
            erD_sndstr("P: play\n\r");
            erD_sndstr("E: erase\n\r");
            erD_sndstr("S: stop\n\r");
            erD_sndstr("M: encoded audio buffer memory dump\n\r");
            erD_sndstr("X: input audio buffer memory dump\n\r");
            erD_sndstr("Y: output audio buffer memory dump\n\r");
            erD_sndstr("V: version\n\r");
            erD_sndstr("\\: restart unit\n\r");
            erD_sndstr("H: help\n\r");
            break;

	}
}


void startTask(void *pData)
{
    INT16U          delay;
    INT16U          xerr;
    char            dByte;

    (void)pData;                                                            /* Prevent compiler warning                        */
  
    
    Tmr_Init();
    iniAudioDvce();
    audioMode = A_STOP;
        
    erD_sndstr("\r\n\CerfPDA audio driver demo\r\n");
    erD_sndstr("Press H for help\r\n");
   
    
	
	while (1) {
        startPlay();
		OSTimeDly(1000);
    
		}
    }


int main(void)
{
	                                        /* Clear bss                                           */
    BSP_Init();

    erD_sndstr("\r\n\r\nInitializing: uC/OS-II");
    OSInit();

    OSTaskCreateExt( startTask,
                     NULL,
                     (OS_STK *)&startTaskStk[START_TASK_STK_SIZE - 1],
                     START_TASK_PRIO,
                     START_TASK_PRIO,
                     (OS_STK *)&startTaskStk[0],
                     START_TASK_STK_SIZE,
                     NULL,
                     OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSStart();
    return 0;
}

