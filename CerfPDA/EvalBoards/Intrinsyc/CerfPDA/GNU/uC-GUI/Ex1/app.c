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

#define  TASK_STK_SIZE        4096
void initimage();
/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

        OS_STK  AppStartTaskStk[TASK_STK_SIZE];
        OS_STK  GuiTaskStk[TASK_STK_SIZE];
		OS_STK  RFDriverInitStk[TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppStartTask(void *p_arg);
static  void  GuiTask(void *p_arg);

/*
*********************************************************************************************************
*                                             C ENTRY POINT
*********************************************************************************************************
*/

int  main (void)
{
    INT8U err;

    BSP_Init();                                 /* Initialize BSP                                      */

    erD_sndstr("\r\nInitialize uC/OS-II...");
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

	OSTaskCreateExt(GuiTask,
					NULL,
					(OS_STK *)&GuiTaskStk[TASK_STK_SIZE-1],
					TASK_GUI_PRIO,
					TASK_GUI_PRIO,
					(OS_STK *)&GuiTaskStk[0],
					TASK_STK_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	Tasks();

                                                /* Give a name to tasks                                */
#if 0                                                
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(OS_IDLE_PRIO,        "Idle task",  &err);
    //OSTaskNameSet(OS_STAT_PRIO,        "Stat task",  &err);
    OSTaskNameSet(TASK_START_APP_PRIO, "Start task", &err);
#endif
#endif

    erD_sndstr("\r\nStart uC/OS-II...");
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

	Tmr_TickInit();                             /* Start timer tick                                    */
    erD_sndstr("\r\nStart timer tick...");
   
//#if OS_TASK_STAT_EN > 0
  //  printf("\r\nStart statistics...");
   // OSStatInit();                               /* Start stats task                                    */
//#endif
                                                /* Give a name to tasks                                */
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(TASK_GUI_PRIO,        "GUI task",  &err);
#endif

	OSTimeDlyHMSM(0,0,10,0);					// Give Some time to cellular connect
	RFDriverInit();
	GPS_Init();

	//Tmr_Init();
	//iniAudioDvce();

	PdaleInterface();
	
	// Montre le dialog d'initialisation
	ShowInitDialog();

	while (1) 
	{
		CheckButtonState(); // Regarde l'�tat des boutons constamment
	}
}

static void  GuiTask (void *p_arg)
{
	// Initialisation d'un GUI
	GUI_Init();
    p_arg = p_arg;                              /* Prevent compiler warning                            */
    while (1)
    {
        GUI_TOUCH_Exec();
        GUI_Exec();
		OSTimeDly(50);
    }
}
