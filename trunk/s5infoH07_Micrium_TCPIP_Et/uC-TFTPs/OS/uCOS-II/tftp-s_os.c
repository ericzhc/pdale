/*
*********************************************************************************************************
*                                               uC/TFTPs
*                                Trivial File Transfer Protocol (server)
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
*                                              TFTP SERVER
*
*                                        RTOS SPECIFIC INTERFACE
*
* Filename      : tftp-s_os.c
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
#include  <tftp-s.h>


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_STK  TFTPs_OS_TaskStk[TFTPs_OS_CFG_TASK_STK_SIZE];   /* Stack for TFTPs task.                                */


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  TFTPs_OS_Task (void  *p_arg);                     /* Server task: wait for clients to connect.            */


/*
*********************************************************************************************************
*                                            TFTPs_OS_Task()
*
* Description : RTOS interface for TFTP server main loop.
*
* Argument(s) : p_arg       argument to pass to the task.
*
* Return(s)   : None.
*
* Caller(s)   : RTOS
*********************************************************************************************************
*/

static  void  TFTPs_OS_Task (void  *p_arg)
{
    TFTPs_Task(p_arg);                                          /* Call TFTP server task body.                          */
}


/*
*********************************************************************************************************
*                                            TFTPs_OS_Init()
*
* Description : Initialize the TFTPs server task.
*
* Argument(s) : p_arg           argument to pass to the task.
*
* Return(s)   : DEF_FAIL        initialize task failed.
*               DEF_OK          initialize task successful.
*
* Caller(s)   : TFTPs_Init()
*
* Note(s)     : The RTOS needs to create TFTPs_OS_Task().
*********************************************************************************************************
*/

CPU_BOOLEAN  TFTPs_OS_Init (void  *p_arg)
{
    CPU_INT08U  err;


#if (OS_TASK_CREATE_EXT_EN > 0)
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreateExt( TFTPs_OS_Task,
                           p_arg,
                                                                /* Set Top-Of-Stack.                                    */
                          &TFTPs_OS_TaskStk[TFTPs_OS_CFG_TASK_STK_SIZE - 1],
                           TFTPs_OS_CFG_TASK_PRIO,
                           TFTPs_OS_CFG_TASK_PRIO,
                          &TFTPs_OS_TaskStk[0],                 /* Set Bottom-Of-Stack.                                 */
                           TFTPs_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #else
    err = OSTaskCreateExt( TFTPs_OS_Task,
                           p_arg,
                          &TFTPs_OS_TaskStk[0],                 /* Set Top-Of-Stack.                                    */
                           TFTPs_OS_CFG_TASK_PRIO,
                           TFTPs_OS_CFG_TASK_PRIO,
                                                                /* Set Bottom-Of-Stack.                                 */
                          &TFTPs_OS_TaskStk[TFTPs_OS_CFG_TASK_STK_SIZE - 1],
                           TFTPs_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #endif
#else
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreate( TFTPs_OS_Task,
                        p_arg,
                       &TFTPs_OS_TaskStk[TFTPs_OS_CFG_TASK_STK_SIZE - 1],
                        TFTPs_OS_CFG_TASK_PRIO);
    #else
    err = OSTaskCreate( TFTPs_OS_Task,
                        p_arg,
                       &TFTPs_OS_TaskStk[0],
                        TFTPs_OS_CFG_TASK_PRIO);
    #endif
#endif

    if (err != OS_NO_ERR) {
        return (DEF_FAIL);
    }

#if (OS_TASK_NAME_SIZE >= 16)
    OSTaskNameSet(TFTPs_OS_CFG_TASK_PRIO, TFTPs_OS_CFG_TASK_NAME, &err);
#endif

    return (DEF_OK);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
