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
#define  TASK_STK_SIZE        2048

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
    printf("Initialize uC/OS-II...\n\r");
    OSInit();                                   /* Initialize uC/OS-II                                 */
     
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
    OSTaskNameSet(OS_TASK_IDLE_PRIO,        "Idle task",  &err);
    OSTaskNameSet(OS_TASK_STAT_PRIO,        "Stat task",  &err);
    OSTaskNameSet(TASK_START_APP_PRIO, "Start task", &err);
#endif


    printf("Start uC/OS-II...\n\r");
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
    INT8U err;
    p_arg = p_arg;                              /* Prevent compiler warning                            */
    printf("Start timer tick...\n\r");
    Tmr_TickInit();                             /* Start timer tick                                    */
#if OS_TASK_STAT_EN > 0
   // printf("Start statistics...\n\r");
    //OSStatInit();                               /* Start stats task                                    */
#endif
	BCR_Init();
	
	
	//GPS_Init();
	//RFDriverInit();
	

    while (1) {                                 /* Task body, always written as an infinite loop.      */
                                                /* Delay task execution for 500 ms                     */
		//TransmitRfBuffer("AYE BEGIN TES LACETS SONT DETACHES\n\r\0");
		//BCRUpdateTask();
		//OSTimeDly(100);

		OSFlagPend(bcFlag, 
			BAR_CODE_AVAILABLE,
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);
		printf("bar Code Read : %s", BCRValue); 
		OSFlagPost(bcFlag, 
			BAR_CODE_CONSUMED, 
			OS_FLAG_SET, 
			&err);

    }
}
