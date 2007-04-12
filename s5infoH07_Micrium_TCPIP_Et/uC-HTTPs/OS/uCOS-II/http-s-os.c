/*
*********************************************************************************************************
*                                               uC/HTTPs
*                                   HyperText Transfer Protocol Server
*
*                          (c) Copyright 2004-2005, Micrium, Inc., Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              HTTP SERVER
*                                        RTOS SPECIFIC INTERFACE
*
* Filename      : http-s-os.c
* Programmer(s) : JJL
* Version       : V1.11
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <ucos_ii.h>
#include  <net.h>
#include  <http-s.h>

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK      HTTPs_OS_TaskStk[HTTPs_OS_TASK_STK_SIZE];  /* Stack for HTTPs task.                */

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void        HTTPs_OS_Task(void *p_arg);

/*
*********************************************************************************************************
*                                             HTTPs_OS_Task()
*
* Description : RTOS interface for HTTP server main loop.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : RTOS
*********************************************************************************************************
*/

static  void  HTTPs_OS_Task (void *p_arg)
{
    (void)p_arg;
    HTTPs_Task();                                 /* Call HTTP server task body.                       */
}

/*
*********************************************************************************************************
*                                             HTTPs_OS_Init()
*
* Description : Initializes the RTOS objects for the HTTP server.
*
* Argument(s) : None.
*
* Return(s)   : DEF_FAIL        Initialize task failed.
*               DEF_OK          Initialize task successful.
*
* Caller(s)   : HTTPs_Init()
*
* Note(s)     : The RTOS needs to create the HTTPs_OS_Task()
*********************************************************************************************************
*/

CPU_BOOLEAN  HTTPs_OS_Init (void)
{
    INT8U err;


#if OS_TASK_CREATE_EXT_EN > 0
    #if OS_STK_GROWTH == 1
    err = OSTaskCreateExt(HTTPs_OS_Task,
                          (void *)0,
                         &HTTPs_OS_TaskStk[HTTPs_OS_TASK_STK_SIZE - 1],  /* Set Top-Of-Stack                     */
                          HTTPs_OS_TASK_PRIO,                            /* Lowest priority level                */
                          HTTPs_OS_TASK_PRIO,
                         &HTTPs_OS_TaskStk[0],                           /* Set Bottom-Of-Stack                  */
                          HTTPs_OS_TASK_STK_SIZE,
                          (void *)0,                                     /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);    /* Enable stack checking + clear stack  */
    #else
    err = OSTaskCreateExt(HTTPs_OS_Task,
                          (void *)0,
                         &HTTPs_OS_TaskStk[0],                           /* Set Top-Of-Stack                     */
                          HTTPs_OS_TASK_PRIO,                            /* Lowest priority level                */
                          HTTPs_OS_TASK_PRIO,
                         &HTTPs_OS_TaskStk[HTTPs_TASK_STK_SIZE - 1],     /* Set Bottom-Of-Stack                  */
                          HTTPs_OS_TASK_STK_SIZE,
                          (void *)0,                                     /* No TCB extension                     */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);    /* Enable stack checking + clear stack  */
    #endif
#else
    #if OS_STK_GROWTH == 1
    err = OSTaskCreate(HTTPs_OS_Task,
                       (void *)0,
                      &HTTPs_OS_TaskStk[HTTPs_OS_TASK_STK_SIZE - 1],
                       HTTPs_OS_TASK_PRIO);
    #else
    err = OSTaskCreate(HTTPs_OS_Task,
                       (void *)0,
                      &HTTPs_OS_TaskStk[0],
                       HTTPs_OS_TASK_PRIO);
    #endif
#endif

    if (err != OS_NO_ERR) {
        return (DEF_FAIL);
    }

#if OS_TASK_NAME_SIZE > 16
    OSTaskNameSet(HTTPs_OS_TASK_PRIO, HTTPS_OS_TASK_NAME, &err);
#endif
    return (DEF_OK);
}

/*
*********************************************************************************************************
*                                         HTTPs_OS_TaskAbort()
*
* Description : This function is called to abort the HTTP server task.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : HTTPs_Task()
*********************************************************************************************************
*/

void  HTTPs_OS_TaskAbort (void)
{
    OSTaskSuspend(OS_PRIO_SELF);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	   		 		  	   		   	 			       	   	  		      		      		  	 	 		  				 	      		   	  		  				 	  	   		      		    		 	       	  	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		      	   	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
