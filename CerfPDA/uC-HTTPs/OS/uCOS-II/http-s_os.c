/*
*********************************************************************************************************
*                                               uC/HTTPs
*                                 HyperText Transfer Protocol (server)
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              HTTP SERVER
*
*                                        RTOS SPECIFIC INTERFACE
*
* Filename      : http-s_os.c
* Version       : V1.85
* Programmer(s) : JDH
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <app_cfg.h>
#include  <net.h>
#include  <http-s.h>


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  HTTPs_OS_TaskStk[HTTPs_OS_CFG_TASK_STK_SIZE];   /* Stack for HTTPs task.                                */


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  HTTPs_OS_Task(void  *p_arg);                      /* Server task: wait for clients to connect.            */


/*
*********************************************************************************************************
*                                            HTTPs_OS_Task()
*
* Description : RTOS interface for HTTP server main loop.
*
* Argument(s) : p_arg       argument to pass to the task.
*
* Return(s)   : None.
*
* Caller(s)   : RTOS
*********************************************************************************************************
*/

static  void  HTTPs_OS_Task (void  *p_arg)
{
    HTTPs_Task(p_arg);                                          /* Call HTTP server task body.                          */
}


/*
*********************************************************************************************************
*                                            HTTPs_OS_Init()
*
* Description : Initialize the HTTPs server task.
*
* Argument(s) : p_arg           argument to pass to the task.
*
* Return(s)   : DEF_FAIL        initialize task failed.
*               DEF_OK          initialize task successful.
*
* Caller(s)   : HTTPs_Init()
*
* Note(s)     : The RTOS needs to create HTTPs_OS_Task().
*********************************************************************************************************
*/

CPU_BOOLEAN  HTTPs_OS_Init (void  *p_arg)
{
    CPU_INT08U  err;


#if (OS_TASK_CREATE_EXT_EN > 0)
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreateExt( HTTPs_OS_Task,
                           p_arg,
                                                                /* Set Top-Of-Stack.                                    */
                          &HTTPs_OS_TaskStk[HTTPs_OS_CFG_TASK_STK_SIZE - 1],
                           HTTPs_OS_CFG_TASK_PRIO,
                           HTTPs_OS_CFG_TASK_PRIO,
                          &HTTPs_OS_TaskStk[0],                 /* Set Bottom-Of-Stack.                                 */
                           HTTPs_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #else
    err = OSTaskCreateExt( HTTPs_OS_Task,
                           p_arg,
                          &HTTPs_OS_TaskStk[0],                 /* Set Top-Of-Stack.                                    */
                           HTTPs_OS_CFG_TASK_PRIO,
                           HTTPs_OS_CFG_TASK_PRIO,
                                                                /* Set Bottom-Of-Stack.                                 */
                          &HTTPs_OS_TaskStk[HTTPs_OS_CFG_TASK_STK_SIZE - 1],
                           HTTPs_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #endif
#else
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreate( HTTPs_OS_Task,
                        p_arg,
                       &HTTPs_OS_TaskStk[HTTPs_OS_CFG_TASK_STK_SIZE - 1],
                        HTTPs_OS_CFG_TASK_PRIO);
    #else
    err = OSTaskCreate( HTTPs_OS_Task,
                        p_arg,
                       &HTTPs_OS_TaskStk[0],
                        HTTPs_OS_CFG_TASK_PRIO);
    #endif
#endif

    if (err != OS_NO_ERR) {
        return (DEF_FAIL);
    }

#if (OS_TASK_NAME_SIZE > 16)
    OSTaskNameSet(HTTPs_OS_CFG_TASK_PRIO, HTTPs_OS_CFG_TASK_NAME, &err);
#endif

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        HTTPs_OS_TaskSuspend()
*
* Description : This function is called to suspend the HTTP server task.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : HTTPs_Task()
*********************************************************************************************************
*/

void  HTTPs_OS_TaskSuspend (void)
{
    OSTaskSuspend(OS_PRIO_SELF);
}


/*
*********************************************************************************************************
*                                           HTTPs_OS_TxDly()
*
* Description : Create a 100 ms delay.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : HTTPs_Tx()
*********************************************************************************************************
*/

void  HTTPs_OS_TxDly (void)
{
    OSTimeDly(100 * OS_TICKS_PER_SEC / 1000);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
