/*
*********************************************************************************************************
*                                                uC/FTPs
*                                    File Transfer Protocol (server)
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
*                                              FTP SERVER
*
*                                        RTOS SPECIFIC INTERFACE
*
* Filename      : ftp-s_os.c
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
#include  <ftp-s.h>


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* Stack for FTPs server task.                          */
static  OS_STK  FTPs_Srv_OS_TaskStk[FTPs_SRV_OS_CFG_TASK_STK_SIZE];

                                                                /* Stack for FTPs control task.                         */
static  OS_STK  FTPs_Ctrl_OS_TaskStk[FTPs_CTRL_OS_CFG_TASK_STK_SIZE];


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  FTPs_Srv_OS_Task(void  *p_arg);                   /* Server task: wait for clients to connect.            */

static  void  FTPs_Ctrl_OS_Task(void  *p_arg);                  /* Control task: control session with the client.       */


/*
*********************************************************************************************************
*                                           FTPs_Srv_OS_Task()
*
* Description : RTOS interface for FTP server main loop.
*
* Argument(s) : p_arg       argument to pass to the task.
*
* Return(s)   : None.
*
* Caller(s)   : RTOS
*********************************************************************************************************
*/

static  void  FTPs_Srv_OS_Task (void  *p_arg)
{
    FTPs_Srv_Task(p_arg);                                       /* Call FTP server main task body.                      */
}


/*
*********************************************************************************************************
*                                          FTPs_Ctrl_OS_Task()
*
* Description : RTOS interface for FTP server control loop.
*
* Argument(s) : p_arg       argument to pass to the task.
*
* Return(s)   : None.
*
* Caller(s)   : RTOS
*********************************************************************************************************
*/

static  void  FTPs_Ctrl_OS_Task (void  *p_arg)
{
    FTPs_Ctrl_Task(p_arg);                                      /* Call FTP server control task body.                   */
}


/*
*********************************************************************************************************
*                                        FTPs_Srv_OS_TaskCreate()
*
* Description : Initializes the FTPs server task.
*
* Argument(s) : p_arg           argument to pass to the task.
*
* Return(s)   : DEF_FAIL        initialize task failed.
*               DEF_OK          initialize task successful.
*
* Caller(s)   : FTPs_Init()
*
* Note(s)     : The RTOS needs to create FTPs_Srv_OS_Task().
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPs_Srv_OS_TaskCreate (void*  p_arg)
{
    CPU_INT08U  err;


#if (OS_TASK_CREATE_EXT_EN > 0)
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreateExt( FTPs_Srv_OS_Task,
                           p_arg,
                                                                /* Set Top-Of-Stack.                                    */
                          &FTPs_Srv_OS_TaskStk[FTPs_SRV_OS_CFG_TASK_STK_SIZE - 1],
                           FTPs_SRV_OS_CFG_TASK_PRIO,
                           FTPs_SRV_OS_CFG_TASK_PRIO,
                          &FTPs_Srv_OS_TaskStk[0],              /* Set Bottom-Of-Stack.                                 */
                           FTPs_SRV_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #else
    err = OSTaskCreateExt( FTPs_Srv_OS_Task,
                           p_arg,
                          &FTPs_Srv_OS_TaskStk[0],              /* Set Top-Of-Stack.                                    */
                           FTPs_SRV_OS_CFG_TASK_PRIO,
                           FTPs_SRV_OS_CFG_TASK_PRIO,
                                                                /* Set Bottom-Of-Stack.                                 */
                          &FTPs_Srv_OS_TaskStk[FTPs_SRV_OS_CFG_TASK_STK_SIZE - 1],
                           FTPs_SRV_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #endif
#else
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreate( FTPs_Srv_OS_Task,
                        p_arg,
                       &FTPs_Srv_OS_TaskStk[FTPs_SRV_OS_CFG_TASK_STK_SIZE - 1],
                        FTPs_SRV_OS_CFG_TASK_PRIO);
    #else
    err = OSTaskCreate( FTPs_Srv_OS_Task,
                        p_arg,
                       &FTPs_Srv_OS_TaskStk[0],
                        FTPs_SRV_OS_CFG_TASK_PRIO);
    #endif
#endif

    if (err != OS_NO_ERR) {
        return (DEF_FAIL);
    }

#if (OS_TASK_NAME_SIZE > 16)
    OSTaskNameSet(FTPs_SRV_OS_CFG_TASK_PRIO, FTPs_SRV_OS_CFG_TASK_NAME, &err);
#endif
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                       FTPs_Ctrl_OS_TaskCreate()
*
* Description : Initializes the FTPs control task.
*
* Argument(s) : p_arg           argument to pass to the task.
*
* Return(s)   : DEF_FAIL        initialize task failed.
*               DEF_OK          initialize task successful.
*
* Caller(s)   : FTPs_Srv_Task()
*
* Note(s)     : The RTOS needs to create FTPs_Ctrl_OS_Task().
*********************************************************************************************************
*/

CPU_BOOLEAN  FTPs_Ctrl_OS_TaskCreate (void  *p_arg)
{
    CPU_INT08U  err;


#if (OS_TASK_CREATE_EXT_EN > 0)
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreateExt( FTPs_Ctrl_OS_Task,
                           p_arg,
                                                                /* Set Top-Of-Stack.                                    */
                          &FTPs_Ctrl_OS_TaskStk[FTPs_CTRL_OS_CFG_TASK_STK_SIZE - 1],
                           FTPs_CTRL_OS_CFG_TASK_PRIO,
                           FTPs_CTRL_OS_CFG_TASK_PRIO,
                          &FTPs_Ctrl_OS_TaskStk[0],             /* Set Bottom-Of-Stack.                                 */
                           FTPs_CTRL_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                          OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #else
    err = OSTaskCreateExt( FTPs_Ctrl_OS_Task,
                           p_arg,
                          &FTPs_Ctrl_OS_TaskStk[0],             /* Set Top-Of-Stack.                                    */
                           FTPs_CTRL_OS_CFG_TASK_PRIO,
                           FTPs_CTRL_OS_CFG_TASK_PRIO,
                                                                /* Set Bottom-Of-Stack.                                 */
                          &FTPs_Ctrl_OS_TaskStk[FTPs_CTRL_OS_CFG_TASK_STK_SIZE - 1],
                           FTPs_CTRL_OS_CFG_TASK_STK_SIZE,
                          (void *)0,                            /* No TCB extension.                                    */
                                                                /* Enable stack checking + clear stack.                 */
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    #endif
#else
    #if (OS_STK_GROWTH == 1)
    err = OSTaskCreate( FTPs_Ctrl_OS_Task,
                        p_arg,
                       &FTPs_Ctrl_OS_TaskStk[FTPs_CTRL_OS_CFG_TASK_STK_SIZE - 1],
                        FTPs_CTRL_OS_CFG_TASK_PRIO);
    #else
    err = OSTaskCreate( FTPs_Ctrl_OS_Task,
                        p_arg,
                       &FTPs_Ctrl_OS_TaskStk[0],
                        FTPs_CTRL_OS_CFG_TASK_PRIO);
    #endif
#endif

    if (err != OS_NO_ERR) {
        return (DEF_FAIL);
    }

#if (OS_TASK_NAME_SIZE > 16)
    OSTaskNameSet(FTPs_CTRL_OS_CFG_TASK_PRIO, FTPs_CTRL_OS_CFG_TASK_NAME, &err);
#endif

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         FTPs_OS_TaskSuspend()
*
* Description : This function is called to suspend any of the two FTP server tasks.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_{Srv, Ctrl}_Task()
*********************************************************************************************************
*/

void  FTPs_OS_TaskSuspend (void)
{
    OSTaskSuspend(OS_PRIO_SELF);
}


/*
*********************************************************************************************************
*                                         FTPs_OS_TaskDelete()
*
* Description : This function is called to terminate any of the three FTP server tasks.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_{Srv, Ctrl}_Task()
*********************************************************************************************************
*/

void  FTPs_OS_TaskDelete (void)
{
    OSTaskDel(OS_PRIO_SELF);
}


/*
*********************************************************************************************************
*                                            FTPs_OS_TxDly()
*
* Description : Create a 100 ms delay.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Caller(s)   : FTPs_Tx()
*********************************************************************************************************
*/

void  FTPs_OS_TxDly (void)
{
    OSTimeDly(100 * OS_TICKS_PER_SEC / 1000);
}
	 	 			 		    	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  	 	 		   	 			       	  	 	 	 	   	  		 	 	  		 		  	 	 					 	 	 	   	   	  		 	 	 			 	    		  	  			   	  		 				 		 					 	  	  	 	 				 		 	 		 		 	 	  		  	  			 				 		 
