/**
*********************************************************************
*   @file       erSipMainTest.c
*   @version    0.1
*   @author     ELR
*   @date       22 Feb 2006 
*
*   Project:    Sip Stack
*
*   @brief      Sip stack main test routine
*
*
*   Revision History:
* 
* 
*
*   COPYRIGHT (C) 2006 Eduardo Luis Romero. Université de Sherbrooke.
*   Québec, Canada.
*
*********************************************************************/

#include <er_types.h>
#include <erLogg.h>
#include <er_sipglobals.h>
#include <er_sipErrCodes.h>
#include <erOs.h>
#include <erLibFn.h>
#include <erSipParser.h>
#include <erScanner.h>
#include <erSipStrMngr.h>
#include <erSipStrDef.h>
#include <erSipMessages.h>
#include <erSipMemMngr.h>
#include <erSipTest.h>
#include <erSipStart.h>
#include <string.h>

#if defined(__ER_OS_WIN32)

#include <stdlib.h>
#include <stdio.h>

#elif defined( __ER_OS_MICROC_OSII)
//#include <includes.h>

//#include <app_cfg.h>
//#include <microC_OSII.h>

#elif defined(__ER_OS_LINUX)
    
#endif

/* //////////////////////////////////////////////////////////////////////////// */
/* define cosntants                        //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */

#define     THIS_FILE               "erSipMainTest.c"


erCHAR  testSipSession[]  = ".\\..\\..\\Debug\\TestSipSessions\\SIPS0A.TXT";
erCHAR  testSipSession2[] = "ide:SIPS0A.TXT";
erCHAR  mybuffer[2048];

#if defined(__ER_OS_WIN32)
FILE*   finp;

#elif defined( __ER_OS_MICROC_OSII)
extern OS_STK  SipMainTaskStk[SIP_MAIN_TASK_STK_SIZE];
extern char    _bss;
extern char    _ebss;
FS_FILE *myfile;

#elif defined(__ER_OS_LINUX)
    
#endif


/* //////////////////////////////////////////////////////////////////////////// */
/* local function prototypes               //////////////////////////////////// */
/* //////////////////////////////////////////////////////////////////////////// */



int main(int argc, char* argv[])
{
    erINT16U nRead;
    erINT16S val;
    erCHAR      *p1;
    erCHAR      *p2;
    erINT32U     dife;

    //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
    //// operating system startup

#if defined(__ER_OS_WIN32)
    /* Win32 case: 
     *      Do nothing
     */
    extern char _bss;
    extern char _ebss;

    testSalloc();                                                       /* string alloc system test                     */
    testMBalloc();                                                      /* memory block alloc system test               */

    /* 
     * Message parsing test section
     */
    erLibStrStoIni();
    erLibMemAllocIni();
    nRead = 0;
    if( (finp = fopen(testSipSession,"rb")) == NULL){
        printf("\r\n Can't open the input file");
        exit (1);
    }
    nRead = (erINT16U)fread(mybuffer,1,2000,finp);
    fclose(finp);
    if( nRead ){
        erRcvdMssgTrnslt(mybuffer, nRead);
    }
    return 0;

#elif defined( __ER_OS_MICROC_OSII)
    size_t  s1;
    erINT32U s2;
    
    p1 = &_bss;
    p2 = &_ebss;
    dife = p2 - p1;

    BSP_IntDisAll();                            /* Disable ALL interrupts to the interrupt controller  */
   // memset (&_bss, 0, &_ebss-&_bss);               /* Clear bss                                           */
    erMemSet(&_bss, 0, &_ebss-&_bss);               /* Clear bss                                           */    
    BSP_Init();                                 /* Initialize the BSP                                  */

    printf("\r\n\r\n");
    erLOG(3, (THIS_FILE, "Initializing: uC/OS-II"));
    erOSInit();
    erLibMemAllocIni();
    erLibStrStoIni();

    erTaskCreate(   SipMainTask,
                    NULL,
                    (OS_STK *)&SipMainTaskStk[SIP_MAIN_TASK_STK_SIZE - 1],
                    SIP_MAIN_TASK_PRIO,
                    (OS_STK *)&SipMainTaskStk[0],
                    SIP_MAIN_TASK_STK_SIZE,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    erOSStart();

#elif defined(__ER_OS_LINUX)
    
#endif

    return 0;
}


#if defined(__ER_OS_WIN32)
void testSuite(void)
{
}


#elif defined( __ER_OS_MICROC_OSII)

void testSuite(void)
{
    erINT16U nRead;


    testSalloc();                                                       /* string alloc system test                     */
    testMBalloc();                                                      /* memory block alloc system test               */

    //FS_Init();
    //int x = FS_IoCtl("ram:",FS_CMD_FORMAT_AUTO,0,0);
    //if (x != 0) {
    //  DBG_PRINT("Cannot format RAM disk.\n");
    //}

    //myfile = FS_FOpen(testSipSession2,"r");                             /* open test file                               */
    //if( myfile == NULL){
    //    printf("\r\n Can't open the input file");
    //    exit (1);
    //}
    //nRead = FS_FRead(mybuffer,1,sizeof(mybuffer)-1,myfile);             /* read the file                                */
    //mybuffer[nRead] = '\0';
    //FS_FClose(myfile);
    //if( nRead ){
    //    erRcvdMssgTrnslt(mybuffer, nRead);
    //}
}

#elif defined(__ER_OS_LINUX)
    
#endif
