/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                                 (c) Copyright 2004, Micrium, Weston, FL
*                                          All Rights Reserved
*
*                                            LogicPD LH79520
*                                              Sample code
* File : APP.C
* By   : Jean J. Labrosse
* Modif: Eduardo Romero    02/2007
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK          AppTaskStartStk[APP_START_TASK_STK_SIZE];
OS_STK          scndTaskStk[SCND_TASK_STK_SIZE];
OS_EVENT       *syncSmaphore;

CPU_INT16U	    sDelay;

    
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void    AppTaskStart(void *p_arg);
static  void    scndTask (void *p_arg);

/*
*********************************************************************************************************
*                                             C ENTRY POINT
*********************************************************************************************************
*/

int  main (void)
{
    INT8U err;

    extern char _bss;
    extern char _ebss;

    BSP_IntDisAll();                            /* Disable ALL interrupts to the interrupt controller  */

    memset (&_bss, 0, &_ebss-&_bss);            /* Clear bss                                           */    
    BSP_SetStackPointers();                     /* Initialize the stacks                               */ 

    BSP_Init();                                     /* Initialize the BSP                              */


    DBG_PRINT("Initializing: uC/OS-II\n\r");

    OSInit();                                   /* Initialize uC/OS-II                                 */

                                                /* Create start task                                   */
    OSTaskCreateExt(AppTaskStart,
                    NULL,
                    (OS_STK *)&AppTaskStartStk[APP_START_TASK_STK_SIZE - 1],
                    APP_START_TASK_PRIO,
                    APP_START_TASK_PRIO,
                    (OS_STK *)&AppTaskStartStk[0],
                    APP_START_TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR );

                                                /* Assign names to created tasks                       */
#if OS_TASK_NAME_SIZE > 11                    
    OSTaskNameSet(APP_START_TASK_PRIO, "Start Task", &err);
#endif

    DBG_PRINT("Starting    : uC/OS-II\n\r");
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

static  void  AppTaskStart (void *p_arg)
{
    NET_ERR      err;                              

    (void)p_arg;                                    /* Prevent compiler warning                        */

    DBG_PRINT("Initializing: Timers\n\r");
    Tmr_Init();                                     /* Start timers                                    */

#if OS_TASK_STAT_EN > 0
    OSStatInit();                                   /* Start stats task                                */
#endif  
  
    syncSmaphore = OSSemCreate(0);
  
    OSTaskCreateExt(scndTask,
                    NULL,
                    (OS_STK *)&scndTaskStk[SCND_TASK_STK_SIZE - 1],
                    SCND_TASK_PRIO,
                    SCND_TASK_PRIO,
                    (OS_STK *)&scndTaskStk[0],
                    SCND_TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
#if OS_TASK_NAME_SIZE > 20
    OSTaskNameSet(SMPLNG_TASK_PRIO,        "Secondary task",  &err);
#endif

 
    LED_Off(0);                                                         /* all leds off                                    */
    printf("\r\n\r\nRunning....\r\n");

    while (TRUE) {                                                      /* Task body, always written as an infinite loop.  */

        LED_Toggle(3);
        printf("\r\n Count = ");
        OSSemPost(syncSmaphore);
        OSTimeDlyHMSM(0, 0, 0, 500);        
    }
}


/*
*********************************************************************************************************
*                                                2ND TASK 
*
*********************************************************************************************************
*/

static  void  scndTask (void *p_arg)
{
    INT8U      err;
    INT16U     count = 0;

    
    (void)p_arg;                                    /* Prevent compiler warning                        */


    while (TRUE) {                                  /* Task body, always written as an infinite loop.  */
        OSSemPend(syncSmaphore, 0, &err);
        printf("%5.5u",count++);
        LED_Toggle(1);
    }
}

CPU_BOOLEAN  HTTPs_ValReq (CPU_CHAR *var, CPU_CHAR **val, CPU_INT32U max_size)
{
        return (DEF_OK);
}

CPU_BOOLEAN  HTTPs_ValRx (CPU_CHAR *var, CPU_CHAR *val)
{
    return (DEF_OK);
}