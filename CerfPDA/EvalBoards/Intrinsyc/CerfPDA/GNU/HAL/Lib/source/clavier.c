/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                            (c) Copyright 2003, 2004, Micrium, Inc., 
Weston, FL
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


#define F1 0x01
#define F2 0x02
#define SHIFT 0x16
#define ESCAPE 0x1b
#define ENTER 0x15
#define CAPSCTRL 0x04
#define NUMCUR 0x05
#define UP 0x11	
#define DOWN 0x12
#define LEFT 0x13
#define RIGHT 0x14
#define POWER 0x03
#define BACKSPACE 0x08
#define TAB 0x09

char lowerCase[51] = {POWER,F1,F2,'a','b','c','d',0,0,0,
					'k','e','f','g','h','i','j',0,0,0,
					'r','l','m','n','o','p','q',0,0,0,
					'y','s','t','u','v','w','x',0,0,0,
					ENTER,'z',SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',0,0,0};

char uperCase[51] = {POWER,F1,F2,'A','B','C','D',0,0,0,
					'K','E','F','G','H','I','J',0,0,0,
					'R','L','M','N','O','P','Q',0,0,0,
					'Y','S','T','U','V','W','X',0,0,0,
					ENTER,'Z',SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',0,0,0};

char shift[51] = {POWER,F1,F2,'1','2','3','4',0,0,0,
					'@','5','6','7','8','9','0',0,0,0,
					'\"','+','-','*','/','=',UP,0,0,0,
					RIGHT,ESCAPE,',','.','?',LEFT,':',0,0,0,
					ENTER,TAB,SHIFT,CAPSCTRL,BACKSPACE,NUMCUR,'\\',0,0,0};

char *curmap;

/*
*********************************************************************************************************
*                                               PROTOTYPES
*********************************************************************************************************
*/

INT8U scanColumn(int columb);
INT8U getkeyNumber(int key);
char getKey(int key, int columb);
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

    BSP_Init();                                 /* Initialize BSP            
                           */

    printf("\r\nInitialize uC/OS-II...");
    OSInit();                                   /* Initialize uC/OS-II       
                           */

                                                /* Create start task         
                           */
    OSTaskCreateExt(AppStartTask,
                    NULL,
                    (OS_STK *)&AppStartTaskStk[TASK_STK_SIZE-1],
                    TASK_START_APP_PRIO,
                    TASK_START_APP_PRIO,
                    (OS_STK *)&AppStartTaskStk[0],
                    TASK_STK_SIZE,
                    NULL,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

                                                /* Give a name to tasks      
                           */
#if 0
#if OS_TASK_NAME_SIZE > 10
    OSTaskNameSet(OS_IDLE_PRIO,        "Idle task",  &err);
    OSTaskNameSet(OS_STAT_PRIO,        "Stat task",  &err);
    OSTaskNameSet(TASK_START_APP_PRIO, "Start task", &err);
#endif
#endif

    printf("\r\nStart uC/OS-II...");
    OSStart();                                  /* Start uC/OS-II            
                           */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                              STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the 
book's text, you MUST
*               initialize the ticker only once multitasking has started.
* Arguments   : p_arg is the argument passed to 'AppStartTask()' by 
'OSTaskCreate()'.
* Notes       : 1) The first line of code is used to prevent a compiler 
warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this 
statement.
*               2) Interrupts are enabled once the task start because the 
I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static void  AppStartTask (void *p_arg)
{
    INT8U err;
	char key;
	
	curmap =  lowerCase;
    p_arg = p_arg;                              /* Prevent compiler warning  
                           */

    printf("\r\nStart timer tick...");
    Tmr_TickInit();                             /* Start timer tick          
                           */

	#if OS_TASK_STAT_EN > 0
		printf("\r\nStart statistics...");
		OSStatInit();                               /* Start stats task          
                           */
	#endif

    printf("\r\nStarting uC/GUI demo...\n");

	INT8U keybline =0;
	INT8U keypress =0;
	INT8U oldkeybline = -1;
	INT8U x = 0;
	INT8U y = 0;
	INT8U columb = -1;
	int antirebonCount = -1;
	printf("line: %x \n\r", KEYBLINE);

	while (1) {                                 /* Task body, always written as 
												an infinite loop.      */

		antirebonCount = 0;						/* Delay task execution for 500 ms                     */
		
		while (antirebonCount < 4)
		{
				
				
				oldkeybline = 0;
				keybline = 0;
				columb = 0;
				x = 0;

				for(x=0;x<7;x++)
				{
					keybline = scanColumn(x);	

					if(keybline!=0)
					{
						
						keypress = keybline;
						oldkeybline = keybline;
						antirebonCount = 0;

						for(y =0; y<8; y++)
						{
							keybline = scanColumn(x);	

							if(oldkeybline == keybline)
							{
								antirebonCount++;
										
							}
											
							
							oldkeybline = keybline;
						}

						if(antirebonCount > 4)
						{
							columb = x;	
							break;
						}
						else
						{
							antirebonCount = 0;
						}
					}
		

				}
			
			
				

				//printf("keybline %x \n\r",keybline);

				

		}

		printf("Key %d press on columb %d \r\n",keypress,columb);
		


		keypress = getkeyNumber(keypress);

		printf("Key %d press on columb %d \r\n",keypress,columb);
		
		key = getKey(keypress,columb);

		switch (key)
		{
		case SHIFT:	curmap = shift;
					break;
		case CAPSCTRL: curmap = uperCase;
					   break;	
		default:	printf("%c \n\r",key);
					curmap = lowerCase;
					break;
		
		}
		
		
		OSTimeDly(300);
		
		//printf("Masque %x \n\r",masque);
	
		

	}

}



char getKey(int key, int columb)
{

	return curmap[(key-1)*10 + (columb)];

}


INT8U getkeyNumber(int key)
{
	INT8U newkey = 0;

	switch(key)
	{
		case 1: newkey = 1;
				break;
		case 2: newkey = 2;
				break;
		case 4: newkey = 3;
				break;
		case 8: newkey = 4;
				break;
		case 16: newkey = 5;
				break;
	}

	return newkey;
}



INT8U scanColumn(int columb)
{	
			
			INT8U masque = 1;

			masque = 1;
			masque <<=columb;

			CERF_PDA_CPLD_KEYB_SCNLOW =0xf0;
			CERF_PDA_CPLD_KEYB_SCNLOW = (masque&0x0f);
			CERF_PDA_CPLD_KEYB_SCNHIGH =0xf0;
			CERF_PDA_CPLD_KEYB_SCNHIGH = ((masque&0xf0)>>4);
			
			//printf("Low  : %x \n",CERF_PDA_CPLD_KEYB_SCNLOW);
			//printf("High : %x \n",CERF_PDA_CPLD_KEYB_SCNHIGH);

			return (( KEYBLINE >> 20) & 0x1F);

}
