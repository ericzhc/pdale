/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                            (c) Copyright 2003, 2004, Micrium, Inc., Weston, FL
*                                           All Rights Reserved
*
*                                              LogicPD Zoom
*                                               Sample code
*
* File    : app.c
* By      : Jean J. Labrosse
* Modif by: Christian Legare
* Modif by: Jean-Denis Hatier
*
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_START_APP_PRIO     5
#define  TASK_STK_SIZE        1024

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

        OS_STK  AppStartTaskStk[TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppStartTask(void *p_arg);

/*
*********************************************************************************************************
*                                             C ENTRY POINT
*********************************************************************************************************
*/

int  main (void)
{
    INT8U err;

    BSP_Init();                                 /* Initialize BSP                                      */

    printf("\r\nInitialize uC/OS-II...");
    OSInit();                                   /* Initialize uC/OS-II                                 */

    printf("\r\nInitialize uC-FS...");
    FS_Init();                                  /* Init the file system                                */

                                                /* Create start task                                   */
    OSTaskCreateExt(AppStartTask,
                    NULL,
                    (OS_STK *)&AppStartTaskStk[TASK_STK_SIZE-1],
                    TASK_START_APP_PRIO,
                    TASK_START_APP_PRIO,
                    (OS_STK *)&AppStartTaskStk[0],
                    TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

                                                /* Give a name to tasks                                */
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(OS_IDLE_PRIO,        "Idle task",  &err);
    OSTaskNameSet(OS_STAT_PRIO,        "Stat task",  &err);
    OSTaskNameSet(TASK_START_APP_PRIO, "Start task", &err);
#endif

    printf("\r\nStart uC/OS-II...");
    OSStart();                                  /* Start uC/OS-II                                      */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static void  AppStartTask (void *p_arg)
{
    p_arg = p_arg;                              /* Prevent compiler warning                            */

    printf("\r\nStart timer tick...");
    Tmr_TickInit();                             /* Start timer tick                                    */

#if OS_TASK_STAT_EN > 0
    printf("\r\nStart statistics...");
    OSStatInit();                               /* Start stats task                                    */
#endif

    printf("\r\nStarting uC/OS-II demo...\n");

    MainTask();

    while (TRUE) {                              /* Task body, always written as an infinite loop.      */
                                                /* Delay task execution for 500 ms                     */
        OSTimeDly(500 * OS_TICKS_PER_SEC / 1000);
    }
}
