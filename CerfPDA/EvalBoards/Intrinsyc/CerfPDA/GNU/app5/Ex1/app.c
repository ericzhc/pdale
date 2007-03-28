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
#define  TASK_GUI_PRIO           6
#define  TASK_STK_SIZE        1024

/* Keypad keys structure
f1 f2 a b c d e pwr
e f g h i j k 
l m n o p q r 
s t u v w x y 
z shit caps/ctrl space num/cur \ enter
*/

#define KEY_F1 0x10
#define KEY_F2 0x11
#define KEY_PWR 0x12
#define KEY_SHIFT 0x13
#define KEY_CAPS 0x14
#define KEY_NUM 0x15
#define KEY_SPACE ' '
#define KEY_BACKSLASH '\\'
#define KEY_ENTER '\n'

char KEYPAD_MASK[5][7] = {{KEY_F1, KEY_F2, 'a', 'b', 'c', 'd', KEY_PWR},
						{'e', 'f', 'g', 'h', 'i', 'j', 'k'},
						{'l', 'm', 'n', 'o', 'p', 'q', 'r'},
						{'s', 't', 'u', 'v', 'w', 'x', 'y'},
						{'z', KEY_SHIFT, KEY_CAPS, KEY_SPACE, KEY_NUM, KEY_BACKSLASH, KEY_ENTER}
						
};

char KEYPAD_CAPS_MASK[5][7] = {{KEY_F1, KEY_F2, 'A', 'B', 'C', 'D', KEY_PWR},
						{'E', 'F', 'G', 'H', 'I', 'J', 'K'},
						{'L', 'M', 'N', 'O', 'P', 'Q', 'R'},
						{'S', 'T', 'U', 'V', 'W', 'X', 'Y'},
						{'Z', KEY_SHIFT, KEY_CAPS, KEY_SPACE, KEY_NUM, KEY_BACKSLASH, KEY_ENTER}
						
};

//bool capslock = false;

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

        OS_STK  AppStartTaskStk[TASK_STK_SIZE];
        OS_STK  GuiTaskStk[TASK_STK_SIZE];

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
#if 0                                                
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(OS_IDLE_PRIO,        "Idle task",  &err);
    OSTaskNameSet(OS_STAT_PRIO,        "Stat task",  &err);
    OSTaskNameSet(TASK_START_APP_PRIO, "Start task", &err);
#endif
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
    INT8U err;

    p_arg = p_arg;                              /* Prevent compiler warning                            */

    printf("\r\nStart timer tick...");
    Tmr_TickInit();                             /* Start timer tick                                    */

#if OS_TASK_STAT_EN > 0
    printf("\r\nStart statistics...");
    OSStatInit();                               /* Start stats task                                    */
#endif

    printf("\r\nStarting uC/GUI demo...\n");
   int i =0;
	while (1) {                                 /* Task body, always written as an infinite loop.      */
                                                /* Delay task execution for 500 ms                     */
		CERF_PDA_CPLD_KEYB_SCNLOW = 1;
		CERF_PDA_CPLD_KEYB_SCNHIGH = 0;
		printf("line: %d \n\r", KEYPAD_MASK[0][i]);
		OSTimeDly(1000);i++;
    }
}
