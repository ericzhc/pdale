/*
*********************************************************************************************************
*                                               uC/TFTPs
*                                 Trivial File Transfer Protocol Server
*
*                             (c) Copyright 2003-2004, Micrium, Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     uC/OS-II RTOS INTERFACE LAYER
*
* File : tftps_os.c
* By   : JJL
*********************************************************************************************************
*/

#include  <includes.h>

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TFTPs_OS_TASK_NAME        "uC/TFTPs  Task"
#define  TFTPs_OS_TASK_NAME_SIZE   15

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

static  OS_STK  TFTPs_OS_TaskStk[TFTPs_OS_TASK_STK_SIZE];

/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void    TFTPs_OS_Task(void *p_arg);

/*$PAGE*/
/*
*********************************************************************************************************
*                                      INITIALIZE TFTPs RTOS OBJECTS
*********************************************************************************************************
*/

CPU_BOOLEAN  TFTPs_OS_Init (void)
{
    CPU_INT08U  err;


#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    err = OSTaskCreateExt(TFTPs_OS_Task,
                          (void *)0,
                         &TFTPs_OS_TaskStk[TFTPs_OS_TASK_STK_SIZE - 1],  /* Set Top-Of-Stack                     */
                          TFTPs_OS_TASK_PRIO,                            /* Lowest priority level                */
                          TFTPs_OS_TASK_PRIO,
                         &TFTPs_OS_TaskStk[0],                           /* Set Bottom-Of-Stack                  */
                          TFTPs_OS_TASK_STK_SIZE,
                          (void *)0,                                     /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);    /* Enable stack checking + clear stack  */
    #else
    err = OSTaskCreateExt(TFTPs_OS_Task,
                          (void *)0,
                         &TFTPs_OS_TaskStk[0],                           /* Set Top-Of-Stack                     */
                          TFTPs_OS_TASK_PRIO,                            /* Lowest priority level                */
                          TFTPs_OS_TASK_PRIO,
                         &TFTPs_OS_TaskStk[TFTPs_TASK_STK_SIZE - 1],     /* Set Bottom-Of-Stack                  */
                          TFTPs_OS_TASK_STK_SIZE,
                          (void *)0,                                     /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);    /* Enable stack checking + clear stack  */
    #endif
#else
    #if OS_STK_GROWTH == 1
    err = OSTaskCreate(TFTPs_OS_Task,
                       (void *)0,
                      &TFTPs_OS_TaskStk[TFTPs_OS_TASK_STK_SIZE - 1],
                       TFTPs_OS_TASK_PRIO);
    #else
    err = OSTaskCreate(TFTPs_OS_Task,
                       (void *)0,
                      &TFTPs_OS_TaskStk[0],
                       TFTPs_OS_TASK_PRIO);
    #endif
#endif

    if (err != OS_NO_ERR) {
        return (DEF_FAIL);
    }

#if OS_TASK_NAME_SIZE > 0 && OS_TASK_NAME_SIZE > TFTPs_OS_TASK_NAME_SIZE
    OSTaskNameSet(TFTPs_OS_TASK_PRIO, TFTPs_OS_TASK_NAME,  &err);
#endif

    return (DEF_OK);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                            TFTP SERVER TASK
*
* Description : 
* Arguments   : p_arg       Pointer to argument(s) passed to task when task is created.  This argument
*                           is not currently used.
*********************************************************************************************************
*/

static  void  TFTPs_OS_Task (void *p_arg)
{
    (void)p_arg;
    TFTPs_Task();                                     /* Execute TFTP server task (should NOT return) */
    while (1) {                                       /* TFTPs_Task() should NEVER return             */
        OSTimeDly(OS_TICKS_PER_SEC);
    }
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	   		 		  	   		   	 			       	   	  		      		      		  	 	 		  				 	      		   	  		  				 	  	   		      		    		 	       	  	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		      	   	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
