/*
*********************************************************************************************************
*                                              uC/TCP-IP
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form for FREE evaluation, for educational
*               use or peaceful research.  If you plan on using uC/TCP-IP in a commercial
*               product you need to contact Micrium to properly license its use in your
*               product.  We provide ALL the source code for your convenience and to help
*               you experience uC/TCP-IP.  The fact that the source code is provided does
*               NOT mean that you can use it without paying a licensing fee.
*
*               Knowledge of the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                   NETWORK OPERATING SYSTEM LAYER
*
*                                          Micrium uC/OS-II
*
* Filename      : net_os.c
* Version       : V1.89
* Programmer(s) : ITJ
*********************************************************************************************************
* Note(s)       : (1) Assumes uC/OS-II is included in the product build.
*
*                 (2) REQUIREs the following uC/OS-II features to be ENABLED :
*
*                         ---- FEATURE ----         --- MINIMUM NUMBER TO CONFIGURE FOR NET/OS PORT ---
*
*                     (a) Semaphores                    NET_OS_NBR_SEM (see 'OS OBJECT DEFINES')
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    NET_OS_MODULE
#include  <net.h>


/*$PAGE*/
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#if ((NET_CFG_LOAD_BAL_EN == DEF_ENABLED) || \
     (defined(NET_TCP_MODULE_PRESENT ))   || \
     (defined(NET_SOCK_MODULE_PRESENT)))

#define  NET_OS_TIMEOUT_PRESENT

#endif


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*
* Note(s) : (1) (a) Signals & locks are implemented using binary   semaphores.
*
*               (b) Queues          are implemented using counting semaphores.
*********************************************************************************************************
*/

                                                                /* -------------------- TASK STKs --------------------- */
static  OS_STK     NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE];
static  OS_STK     NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE];


                                                                /* --------------------- SIGNALS ---------------------- */
static  OS_EVENT  *NetOS_InitSignalPtr;
static  OS_EVENT  *NetOS_NIC_TxRdySignalPtr;

#if    (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
static  OS_EVENT  *NetOS_TxSuspendSignalPtr;
#endif

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  OS_EVENT  *NetOS_Sock_ConnReqSignalPtr[NET_SOCK_CFG_NBR_SOCK];
static  OS_EVENT  *NetOS_Sock_ConnAcceptSignalPtr[NET_SOCK_CFG_NBR_SOCK];
static  OS_EVENT  *NetOS_Sock_ConnCloseSignalPtr[NET_SOCK_CFG_NBR_SOCK];
#endif
#endif

                                                                /* ---------------------- LOCKS ----------------------- */
static  OS_EVENT  *NetOS_LockPtr;


                                                                /* --------------------- QUEUES ----------------------- */
static  OS_EVENT  *NetOS_IF_RxQPtr;


                                                                /* ---------------------- SEMS ------------------------ */
#ifdef  NET_TCP_MODULE_PRESENT
static  OS_EVENT  *NetOS_TCP_RxQ_SemPtr[NET_TCP_CFG_NBR_CONN];
static  OS_EVENT  *NetOS_TCP_TxQ_SemPtr[NET_TCP_CFG_NBR_CONN];
#endif

#ifdef  NET_SOCK_MODULE_PRESENT
static  OS_EVENT  *NetOS_Sock_RxQ_SemPtr[NET_SOCK_CFG_NBR_SOCK];
#endif


                                                                /* ---------------------- TMRS ------------------------ */
#if    (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
static  INT16U     NetOS_TxSuspendTimeout_tick;
#endif

#ifdef  NET_TCP_MODULE_PRESENT
static  INT16U     NetOS_TCP_RxQ_TimeoutTbl_tick[NET_TCP_CFG_NBR_CONN];
static  INT16U     NetOS_TCP_TxQ_TimeoutTbl_tick[NET_TCP_CFG_NBR_CONN];
#endif

#ifdef  NET_SOCK_MODULE_PRESENT
static  INT16U     NetOS_Sock_RxQ_TimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];

#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
static  INT16U     NetOS_Sock_ConnReqTimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];
static  INT16U     NetOS_Sock_ConnAcceptTimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];
static  INT16U     NetOS_Sock_ConnCloseTimeoutTbl_tick[NET_SOCK_CFG_NBR_SOCK];
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                        /* ---- NETWORK TIMER MANAGEMENT FUNCTIONS ---- */
static  void        NetOS_Tmr_Task           (void        *p_data);


                                                                        /* ---- NETWORK INTERFACE LAYER FUNCTIONS ----- */
static  void        NetOS_IF_RxTask          (void        *p_data);


                                                                        /* - NETWORK/OS TIMEOUT CALCULATION FUNCTIONS - */
#ifdef  NET_OS_TIMEOUT_PRESENT

static  INT16U      NetOS_TimeoutCalc_OS_tick(CPU_INT32U   timeout_ms,
                                              NET_ERR     *perr);

static  CPU_INT32U  NetOS_TimeoutCalc_ms     (INT16U       os_tick);

#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  NET_OS_CFG_IF_RX_TASK_PRIO
#error  "NET_OS_CFG_IF_RX_TASK_PRIO            not #define'd in 'app_cfg.h'"
#error  "                                [MUST be  >= 0]                   "
#elif   (NET_OS_CFG_IF_RX_TASK_PRIO < 0)
#error  "NET_OS_CFG_IF_RX_TASK_PRIO      illegally #define'd in 'app_cfg.h'"
#error  "                                [MUST be  >= 0]                   "
#endif


#ifndef  NET_OS_CFG_TMR_TASK_PRIO
#error  "NET_OS_CFG_TMR_TASK_PRIO              not #define'd in 'app_cfg.h'"
#error  "                                [MUST be  >= 0]                   "
#elif   (NET_OS_CFG_TMR_TASK_PRIO < 0)
#error  "NET_OS_CFG_TMR_TASK_PRIO        illegally #define'd in 'app_cfg.h'"
#error  "                                [MUST be  >= 0]                   "
#endif



#ifndef  NET_OS_CFG_TMR_TASK_STK_SIZE
#error  "NET_OS_CFG_TMR_TASK_STK_SIZE          not #define'd in 'app_cfg.h'"
#error  "                                [MUST be  > 0]                    "
#elif   (NET_OS_CFG_TMR_TASK_STK_SIZE < 1)
#error  "NET_OS_CFG_TMR_TASK_STK_SIZE    illegally #define'd in 'app_cfg.h'"
#error  "                                [MUST be  > 0]                    "
#endif


#ifndef  NET_OS_CFG_IF_RX_TASK_STK_SIZE
#error  "NET_OS_CFG_IF_RX_TASK_STK_SIZE        not #define'd in 'app_cfg.h'"
#error  "                                [MUST be  > 0]                    "
#elif   (NET_OS_CFG_IF_RX_TASK_STK_SIZE < 1)
#error  "NET_OS_CFG_IF_RX_TASK_STK_SIZE  illegally #define'd in 'app_cfg.h'"
#error  "                                [MUST be  > 0]                    "
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                          NETWORK FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            NetOS_Init()
*
* Description : (1) Perform network/OS initialization :
*
*                   (a) Implement  network initialization signal by creating a counting semaphore.
*
*                       Initialize network initialization signal with no signal by setting the semaphore 
*                       count to 0 to block the initialization signal semaphore.
*
*                   (b) Implement global network lock by creating a binary semaphore.
*
*                       Initialize network lock as released by setting the semaphore count to 1.
*
*                   (c) Implement  network transmit suspend signal by creating a counting semaphore.
*
*                       Initialize network transmit suspend signal with no signal by setting the semaphore 
*                       count to 0 to block the transmit suspend signal semaphore.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                         Network/OS initialization successful.
*
*                               NET_OS_ERR_INIT_SIGNAL                  Network    initialization signal 
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SIGNAL_NAME             Network    initialization signal name
*                                                                           NOT successfully configured.
*
*                               NET_OS_ERR_INIT_LOCK                    Network    lock           signal 
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_LOCK_NAME               Network    lock           signal name
*                                                                           NOT successfully configured.
*
*                               NET_OS_ERR_INIT_TX_SUSPEND              Network  transmit suspend signal
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_TX_SUSPEND_NAME         Network  transmit suspend signal name
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_TX_SUSPEND_TIMEOUT      Network  transmit suspend signal timeout
*                                                                           NOT successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetOS_Init (NET_ERR  *perr)
{
#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    INT8U    os_err;
#endif
#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
    NET_ERR  net_err;
#endif


    NetOS_InitSignalPtr = OSSemCreate((INT16U)0);           /* Create network initialization signal (see Note #1a).     */
    if (NetOS_InitSignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_SIGNAL;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_InitSignalPtr,
                   (INT8U    *) NET_INIT_NAME,
                   (INT8U    *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_SIGNAL_NAME;
        return;
    }
#endif


    NetOS_LockPtr = OSSemCreate((INT16U)1);                 /* Create network lock (see Note #1b).                      */
    if (NetOS_LockPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_LOCK;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_LockPtr,
                   (INT8U    *) NET_LOCK_NAME,
                   (INT8U    *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_LOCK_NAME;
        return;
    }
#endif


#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
    NetOS_TxSuspendSignalPtr = OSSemCreate((INT16U)0);      /* Create network transmit suspend signal (see Note #1c).   */
    if (NetOS_TxSuspendSignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_TX_SUSPEND;
        return;
    }
                                                            /* Initialize net transmit suspend signal timeout value.    */
    NetOS_TxSuspendTimeoutSet(NET_CFG_TX_SUSPEND_TIMEOUT_MS, &net_err);
    if (net_err != NET_ERR_NONE) {
       *perr = NET_OS_ERR_INIT_TX_SUSPEND_TIMEOUT;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_TxSuspendSignalPtr,
                   (INT8U    *) NET_TX_SUSPEND_NAME,
                   (INT8U    *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_TX_SUSPEND_NAME;
        return;
    }
#endif
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetOS_InitWait()
*
* Description : Wait on signal indicating network initialization is complete.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Initialization signal     received.
*                               NET_OS_ERR_INIT                 Initialization signal NOT received.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler(),
*               NetTmr_TaskHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network initialization signal MUST be acquired--i.e. MUST wait for access; do NOT timeout.
*
*                   Failure to acquire signal will prevent network task(s) from running.
*********************************************************************************************************
*/

void  NetOS_InitWait (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend(NetOS_InitSignalPtr, (INT16U)0, &os_err);         /* Wait until network initialization complete.          */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_OS_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_OS_ERR_INIT;
             break;
    }
}


/*
*********************************************************************************************************
*                                         NetOS_InitSignal()
*
* Description : Signal that network initialization is complete.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Network initialization     successfully signaled.
*                               NET_OS_ERR_INIT_SIGNALD         Network initialization NOT successfully signaled.
*
* Return(s)   : none.
*
* Caller(s)   : Net_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network initialization MUST be signaled--i.e. MUST signal without failure.
*
*                   Failure to signal will prevent network task(s) from running.
*********************************************************************************************************
*/

void  NetOS_InitSignal (NET_ERR  *perr)
{
    INT8U  os_err;


    os_err = OSSemPost(NetOS_InitSignalPtr);                    /* Signal network initialization complete.              */
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_SIGNALD;
    }

    *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            NetOS_Lock()
*
* Description : Acquire mutually exclusive access to network protocol suite.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Network access     acquired.
*                               NET_OS_ERR_LOCK                 Network access NOT acquired.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network access MUST be acquired--i.e. MUST wait for access; do NOT timeout.
*
*                   Failure to acquire network access will prevent network task(s)/operation(s)
*                   from functioning.
*********************************************************************************************************
*/

void  NetOS_Lock (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend(NetOS_LockPtr, (INT16U)0, &os_err);               /* Acquire network access (see Note #1).                */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_OS_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_OS_ERR_LOCK;
             break;
    }
}


/*
*********************************************************************************************************
*                                           NetOS_Unlock()
*
* Description : Release mutually exclusive access to network protocol suite.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
*               This function is an INTERNAL network protocol suite function & SHOULD NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network access MUST be released--i.e. MUST unlock access without failure.
*
*                   Failure to release network access will prevent network task(s)/operation(s)
*                   from functioning.
*********************************************************************************************************
*/

void  NetOS_Unlock (void)
{
   (void)OSSemPost(NetOS_LockPtr);                              /* Release network access.                              */
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_TxSuspendWait()
*
* Description : Wait on network transmit suspend signal.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_TxSuspend().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Network transmit suspend waits until :
*
*                   (a) Signaled
*                   (b) Timed out
*                   (c) Any OS fault occurs
*
*               (2) Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  NetOS_TxSuspendWait (void)
{
    INT8U  os_err;

                                                                /* Wait on network transmit suspend signal.               */
    OSSemPend(NetOS_TxSuspendSignalPtr, NetOS_TxSuspendTimeout_tick, &os_err);

   (void)os_err;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TxSuspendSignal()
*
* Description : Signal network transmit suspend.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Net_TxSuspendSignal().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) Failure to signal network transmit suspend signal will cause network transmit suspends
*                   to timeout.
*
*                   See also 'NetOS_TxSuspendWait()  Note #1b'.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  NetOS_TxSuspendSignal (void)
{
   (void)OSSemPost(NetOS_TxSuspendSignalPtr);                   /* Signal network transmit suspend.                     */
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TxSuspendTimeoutSet()
*
* Description : Set network transmit suspend timeout value.
*
* Argument(s) : timeout_ms      Timeout value (in milliseconds).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_NONE                    Network transmit suspend timeout successfully set.
*
*                                                               --- RETURNED BY NetOS_TimeoutCalc_OS_tick() : ----
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
void  NetOS_TxSuspendTimeoutSet (CPU_INT32U   timeout_ms,
                                 NET_ERR     *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_TxSuspendTimeout_tick = os_tick;                      /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_TxSuspendTimeoutGet_ms()
*
* Description : Get network transmit suspend timeout value.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_ERR_NONE                    Network transmit suspend timeout 
*                                                                   successfully returned.
*
* Return(s)   : Network transmit suspend timeout value (in milliseconds).
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#if (NET_CFG_LOAD_BAL_EN == DEF_ENABLED)
CPU_INT32U  NetOS_TxSuspendTimeoutGet_ms (NET_ERR  *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT16U  timeout_ms;


    CPU_CRITICAL_ENTER();
    os_tick    =  NetOS_TxSuspendTimeout_tick;                  /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = (CPU_INT16U)NetOS_TimeoutCalc_ms(os_tick);     /* Calculate   timeout value (in milliseconds).         */

   *perr       =  NET_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                 NETWORK TIMER MANAGEMENT FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_Tmr_Init()
*
* Description : (1) Perform Timer/OS initialization :
*
*                   (a) Create network timer task
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Network timer/OS initialization successful.
*                               NET_OS_ERR_INIT_TMR_TASK        Network timer task      NOT successfully
*                                                                   initialized.
*                               NET_OS_ERR_INIT_TMR_TASK_NAME   Network timer task name NOT successfully
*                                                                   configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetTmr_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetOS_Tmr_Init (NET_ERR  *perr)
{
    INT8U  os_err;

                                                                            /* Create NetOS_Tmr_Task() [see Note #1a].  */
#if (OS_TASK_CREATE_EXT_EN == 1)

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_Tmr_Task,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE - 1],
                             (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_TMR_TASK_PRIO,   /* Set task id same as task prio.           */
                             (OS_STK        * )&NetOS_Tmr_TaskStk[0],
                             (INT32U          ) NET_OS_CFG_TMR_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_Tmr_Task,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_Tmr_TaskStk[0],
                             (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_TMR_TASK_PRIO,   /* Set task id same as task prio.           */
                             (OS_STK        * )&NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE - 1],
                             (INT32U          ) NET_OS_CFG_TMR_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#endif

#else

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreate((void (*)(void *)) NetOS_Tmr_Task,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_Tmr_TaskStk[NET_OS_CFG_TMR_TASK_STK_SIZE - 1],
                          (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO);
#else
    os_err = OSTaskCreate((void (*)(void *)) NetOS_Tmr_Task,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_Tmr_TaskStk[0],
                          (INT8U           ) NET_OS_CFG_TMR_TASK_PRIO);
#endif

#endif
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_TMR_TASK;
        return;
    }


#if (OS_TASK_NAME_SIZE >= NET_TASK_NAME_SIZE_MAX)
    OSTaskNameSet((INT8U  ) NET_OS_CFG_TMR_TASK_PRIO,
                  (INT8U *) NET_TMR_TASK_NAME,
                  (INT8U *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_TMR_TASK_NAME;
        return;
    }
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          NetOS_Tmr_Task()
*
* Description : OS-dependent shell task to schedule & run Timer Task Handler.
*
*               (1) Shell task's primary purpose is to schedule & run NetTmr_TaskHandler(); shell task
*                   should run NetTmr_TaskHandler() at NET_TMR_CFG_TASK_FREQ rate forever (i.e. shell 
*                   task should NEVER exit).
*
*
* Argument(s) : p_data      Pointer to task initialization data (required by uC/OS-II).
*
* Return(s)   : none.
*
* Created by  : NetOS_Tmr_Init().
*
* Note(s)     : (2) Assumes OS_TICKS_PER_SEC frequency is greater than NET_TMR_CFG_TASK_FREQ.  Otherwise,
*                   timer task scheduling rate will NOT be correct.
*********************************************************************************************************
*/

static  void  NetOS_Tmr_Task (void  *p_data)
{
    INT16U  dly;


   (void)&p_data;                                               /* Prevent compiler warning.                            */

    dly = OS_TICKS_PER_SEC / NET_TMR_CFG_TASK_FREQ;             /* Delay task at NET_TMR_CFG_TASK_FREQ rate.            */

    while (DEF_ON) {
        OSTimeDly(dly);

        NetTmr_TaskHandler();
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                  NETWORK INTERFACE CARD FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_NIC_Init()
*
* Description : (1) Perform NIC/OS initialization :
*
*                   (a) Implement NIC transmit ready signal by creating a binary semaphore.
*
*                       Initialize transmit ready signal based on configuration :
*
*                       (1) NET_NIC_CFG_TX_RDY_INIT_VAL configured to initialize the NIC-transmit-ready
*                           OS object with one of the following values :
*
*                           (A) 0, for NIC driver's that implement the NIC's transmit-ready status using
*                                  the Transmit Empty    interrupt.
*
*                           (B) 1, for NIC driver's that implement the NIC's transmit-ready status using 
*                                  the Transmit Complete interrupt.
*
*                       See also 'net_cfg  NETWORK INTERFACE CARD CONFIGURATION  Note #2'.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                     NIC/OS initialization successful.
*                               NET_OS_ERR_INIT_NIC_TX_RDY          NIC transmit ready signal NOT successfully
*                                                                       initialized.
*                               NET_OS_ERR_INIT_NIC_TX_RDY_NAME     NIC transmit ready name   NOT successfully
*                                                                       configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_Init().
*
*               This function is a network protocol suite to network interface controller (NIC)  function &
*               SHOULD be called only by appropriate network interface controller initialization function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  NetOS_NIC_Init (NET_ERR  *perr)
{
#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    INT8U  os_err;
#endif

                                                                /* Create transmit ready signal (see Note #1a).         */
    NetOS_NIC_TxRdySignalPtr = OSSemCreate((INT16U)NET_NIC_CFG_TX_RDY_INIT_VAL);        
    if (NetOS_NIC_TxRdySignalPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_NIC_TX_RDY;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_NIC_TxRdySignalPtr,
                   (INT8U    *) NET_NIC_TX_RDY_NAME,
                   (INT8U    *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_NIC_TX_RDY_NAME;
        return;
    }
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_NIC_TxRdyWait()
*
* Description : Wait on NIC transmit ready signal.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_NIC_ERR_NONE                        NIC transmit ready signal     received.
*                               NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT       NIC transmit ready signal NOT received
*                                                                           by timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Tx(),
*               NetIF_Char_Tx().
*
*               This function is a network interface (IF) to network interface controller (NIC) function 
*               & SHOULD be called only by appropriate network interface function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on NIC transmit ready signal forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT error on transmit ready
*                   timeout.  Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

void  NetOS_NIC_TxRdyWait (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend(NetOS_NIC_TxRdySignalPtr, (INT16U)0, &os_err);    /* Wait on NIC transmit ready signal.                   */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_NIC_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_NIC_ERR_TX_RDY_SIGNAL_TIMEOUT;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_NIC_TxRdySignal()
*
* Description : Signal NIC transmit that NIC transmit buffer is ready.
*
* Argument(s) : psignal     Pointer to value that signals NIC transmit.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_TxISR_Handler().
*
*               This function is a network protocol suite to network interface controller (NIC) function
*               & SHOULD be called only by appropriate network interface controller function(s).
*
* Note(s)     : (1) #### NIC transmit ready MUST be signaled--i.e. MUST signal without failure.
*
*                   Failure to signal NIC transmit ready will prevent NIC from transmitting packets.
*********************************************************************************************************
*/

void  NetOS_NIC_TxRdySignal (void)
{
    INT8U  os_err;

                                                                /* Signal NIC transmit that transmit ready.             */
    os_err = OSSemPost(NetOS_NIC_TxRdySignalPtr);

   (void)os_err;                                                /* See Note #1.                                         */
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                  NETWORK INTERFACE LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           NetOS_IF_Init()
*
* Description : (1) Perform network interface/OS initialization :
*
*                   (a) Implement network interface queue by creating a counting semaphore.
*
*                       Initialize network interface queue with no received packets by setting the semaphore
*                       count to 0 to block the network interface queue semaphore.
*
*                   (b) Create network IF Receive Task.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                     Network interface/OS initialization
*                                                                           successful.
*                               NET_OS_ERR_INIT_IF_RX_Q             Network interface receive queue signal
*                                                                       NOT successfully initialized.
*                               NET_OS_ERR_INIT_IF_RX_Q_NAME        Network interface receive queue name
*                                                                       NOT successfully configured.
*                               NET_OS_ERR_INIT_IF_RX_TASK          Network interface receive task
*                                                                       NOT successfully initialized.
*                               NET_OS_ERR_INIT_IF_RX_TASK_NAME     Network interface receive task  name
*                                                                       NOT successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_Pkt_Init(),
*               NetIF_Char_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
void  NetOS_IF_Init (NET_ERR  *perr)
{
    INT8U  os_err;


                                                                            /* Create receive queue     (see Note #1a). */
    NetOS_IF_RxQPtr = OSSemCreate((INT16U)0);
    if (NetOS_IF_RxQPtr == (OS_EVENT *)0) {
       *perr = NET_OS_ERR_INIT_IF_RX_Q;
        return;
    }

#if (OS_EVENT_NAME_SIZE >= NET_OBJ_NAME_SIZE_MAX)
    OSEventNameSet((OS_EVENT *) NetOS_IF_RxQPtr,
                   (INT8U    *) NET_IF_RX_Q_NAME,
                   (INT8U    *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_IF_RX_Q_NAME;
        return;
    }
#endif


                                                                            /* Create NetOS_IF_RxTask() [see Note #1b]. */
#if (OS_TASK_CREATE_EXT_EN == 1)

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_IF_RxTask,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE - 1],
                             (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_IF_RX_TASK_PRIO, /* Set task id same as task prio.           */
                             (OS_STK        * )&NetOS_IF_RxTaskStk[0],
                             (INT32U          ) NET_OS_CFG_IF_RX_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
    os_err = OSTaskCreateExt((void (*)(void *)) NetOS_IF_RxTask,
                             (void          * ) 0,
                             (OS_STK        * )&NetOS_IF_RxTaskStk[0],
                             (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO,
                             (INT16U          ) NET_OS_CFG_IF_RX_TASK_PRIO, /* Set task id same as task prio.           */
                             (OS_STK        * )&NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE - 1],
                             (INT32U          ) NET_OS_CFG_IF_RX_TASK_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#endif

#else

#if (OS_STK_GROWTH == 1)
    os_err = OSTaskCreate((void (*)(void *)) NetOS_IF_RxTask,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_IF_RxTaskStk[NET_OS_CFG_IF_RX_TASK_STK_SIZE - 1],
                          (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO);
#else
    os_err = OSTaskCreate((void (*)(void *)) NetOS_IF_RxTask,
                          (void          * ) 0,
                          (OS_STK        * )&NetOS_IF_RxTaskStk[0],
                          (INT8U           ) NET_OS_CFG_IF_RX_TASK_PRIO);
#endif

#endif
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_IF_RX_TASK;
        return;
    }


#if (OS_TASK_NAME_SIZE >= NET_TASK_NAME_SIZE_MAX)
    OSTaskNameSet((INT8U  ) NET_OS_CFG_IF_RX_TASK_PRIO,
                  (INT8U *) NET_IF_RX_TASK_NAME,
                  (INT8U *)&os_err);
    if (os_err != OS_NO_ERR) {
       *perr = NET_OS_ERR_INIT_IF_RX_TASK_NAME;
        return;
    }
#endif


   *perr = NET_OS_ERR_NONE;
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_IF_RxTask()
*
* Description : OS-dependent shell task to schedule & run Network Interface Receive Task Handler.
*
*               (1) Shell task's primary purpose is to schedule & run NetIF_RxTaskHandler() forever;
*                   (i.e. shell task should NEVER exit).
*
*
* Argument(s) : p_data      Pointer to task initialization data (required by uC/OS-II).
*
* Return(s)   : none.
*
* Created by  : NetOS_IF_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  NetOS_IF_RxTask (void  *p_data)
{
   (void)&p_data;                                               /* Prevent compiler warning.                            */

    NetIF_RxTaskHandlerInit();
    while (DEF_ON) {
        NetIF_RxTaskHandler();
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_IF_RxTaskWait()
*
* Description : Wait on network interface receive queue for NIC receive signal.
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Signal received from NIC receive ISR.
*                               NET_IF_ERR_RX_Q_EMPTY           Network interface receive queue empty.
*
* Return(s)   : none.
*
* Caller(s)   : NetIF_RxTaskHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on network interface receive queue until signaled
*                   (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_IF_ERR_RX_Q_EMPTY error on receive queue timeout.
*                   Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

void  NetOS_IF_RxTaskWait (NET_ERR  *perr)
{
    INT8U  os_err;


    OSSemPend(NetOS_IF_RxQPtr, (INT16U)0, &os_err);             /* Wait on network interface receive queue.             */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_IF_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_IF_ERR_RX_Q_EMPTY;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_IF_RxTaskSignal()
*
* Description : Signal network interface receive task of NIC receive ISR.
*
* Argument(s) : psignal     Pointer to signal value to post to network interface receive queue.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_IF_ERR_NONE                 Network interface receive queue successfully
*                                                                   signaled.
*                               NET_IF_ERR_RX_Q_FULL            Network interface receive queue full.
*                               NET_IF_ERR_RX_Q_SIGNAL          Network interface receive queue signal failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetNIC_RxISR_Handler(),  for packet   -based NICs.
*               NetNIC_RxPktHandler(),   for character-based NICs.
*
*               This function is a network protocol suite to network interface controller (NIC) function
*               & SHOULD be called only by appropriate network interface controller function(s).
*
* Note(s)     : (1) To balance network receive versus transmit packet loads for certain network connection
*                   types (e.g. stream-type connections), network receive & transmit packets SHOULD be 
*                   handled in an APPROXIMATELY balanced ratio.
*
*                   (b) To implement network receive versus transmit load balancing :
*
*                       (1) The availability of network receive packets MUST be managed at the network 
*                           interface layer :
*
*                           (A) Increment the number of available network receive packets queued for
*                               each network packet received.
*
*                   See also 'NETWORK RECEIVE PACKET MACRO'S  Note #1'.
*********************************************************************************************************
*/

void  NetOS_IF_RxTaskSignal (NET_ERR  *perr)
{
#if ((NET_CFG_LOAD_BAL_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif
    INT8U   os_err;


    os_err = OSSemPost(NetOS_IF_RxQPtr);                        /* Signal network interface receive queue.              */

    switch (os_err) {
        case OS_NO_ERR:
                                                                /* Increment number of queued receive packets ...       */
             NET_RX_PKT_INC();                                  /* ... available (see Note #1b1A).                      */
            *perr = NET_IF_ERR_NONE;
             break;


        case OS_SEM_OVF:
            *perr = NET_IF_ERR_RX_Q_FULL;
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_IF_ERR_RX_Q_SIGNAL;
             break;
    }
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                          INTERNET CONTROL MESSAGE PROTOCOL LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        NetOS_ICMP_TxMsgReq()
*
* Description : Transmit ICMP Request Message.
*
*               (1) NetOS_ICMP_TxMsgReq() is the correct API function for network & end-user applications to
*                   transmit ICMP Request Messages (see also 'net_icmp.c  NetICMP_TxMsgReq()  Note #3').
*
*               (2) See 'net_icmp.h  ICMP MESSAGE TYPES & CODES  Notes #2 & #3' for supported ICMP message
*                   types & codes.
*
*
* Argument(s) : type        ICMP Request Message type (see Note #2) :
*
*                               NET_ICMP_MSG_TYPE_ECHO_REQ
*                               NET_ICMP_MSG_TYPE_TS_REQ
*                               NET_ICMP_MSG_TYPE_ADDR_MASK_REQ
*
*               code        ICMP Request Message code (see Note #2).
*
*               TOS         Specific TOS to transmit IP packet
*                               (see 'net_ip.h  IP HEADER TYPE OF SERVICE (TOS) DEFINES').
*
*               TTL         Specific TTL to transmit IP packet (see 'net_ip.h  IP HEADER DEFINES').
*
*               addr_dest   Destination IP address.
*
*               flags       Flags to select transmit options; bit-field flags logically OR'd :
*
*                               NET_IP_FLAG_NONE                No  IP transmit flags selected.
*                               NET_IP_FLAG_TX_DONT_FRAG        Set IP 'Don't Frag' flag.
*
*               popts       Pointer to one or more IP options configuration data structures :
*
*                               NULL                            NO IP transmit options configuration.
*                               NET_IP_OPT_CFG_ROUTE_TS         Route &/or Internet Timestamp options configuration.
*                               NET_IP_OPT_CFG_SECURITY         Security options configuration
*                                                                   (see 'net_ip.h  Note #1f').
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                                                               ------ RETURNED BY NetICMP_TxMsgReq() : ------
*                               NET_ICMP_ERR_NONE               ICMP Request Message successfully transmitted.
*                               NET_ERR_INIT_INCOMPLETE         Network initialization NOT complete.
*                               NET_ERR_TX                      Transmit error; packet discarded.
*
* Return(s)   : ICMP Request Message's Identification (ID) & Sequence Numbers, if NO errors.
*
*               NULL                   Identification (ID) & Sequence Numbers, otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : (3) (a) RFC #792 states that the Identifier field is an "aid in matching [requests] and
*                       replies, may be zero ... For example, the identifier might be used like a port
*                       in TCP or UDP to identify a session" (Sections 'Echo or Echo Reply Message :
*                       Identifier, Description' & 'Timestamp or Timestamp Reply Message : Identifier,
*                       Description').
*
*                   (b) Use uC/OS-II task priority number as ICMP Request Message Identification field.
*
*               (4) #### ICMP Receive Error/Reply Messages NOT yet implemented.
*
*                   See also 'net_icmp.c  NetICMP_Rx()  Note #4'.
*********************************************************************************************************
*/
/*$PAGE*/
NET_ICMP_REQ_ID_SEQ  NetOS_ICMP_TxMsgReq (CPU_INT08U    type,
                                          CPU_INT08U    code,
                                          NET_IP_TOS    TOS,
                                          NET_IP_TTL    TTL,
                                          NET_IP_ADDR   addr_dest,
                                          CPU_INT16U    flags,
                                          void         *popts,
                                          void         *p_data,
                                          CPU_INT16U    data_len,
                                          NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR               cpu_sr;
#endif
    CPU_INT16U           id;
    NET_ICMP_REQ_ID_SEQ  id_seq;


    CPU_CRITICAL_ENTER();
    id     = (CPU_INT16U)OSTCBCur->OSTCBPrio;                   /* Set task prio as ICMP Req Msg id (see Note #3b).     */
    CPU_CRITICAL_EXIT();

    id_seq =  NetICMP_TxMsgReq(type, code, id, TOS, TTL, addr_dest, flags, popts, p_data, data_len, perr);

    return (id_seq);
}


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                            TRANSMISSION CONTROL PROTOCOL LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_TCP_Init()
*
* Description : (1) Perform TCP/OS initialization :
*
*                   (a) Create TCP connection receive  queue binary semaphores :
*
*                       (1) Initialize TCP connection receive queue binary semaphores with no received 
*                           packets  by setting the semaphore count to 0.
*                       (2) Initialize TCP connection receive queue timeout values.
*
*                   (b) Create TCP connection transmit queue binary semaphores :
*
*                       (1) Initialize TCP connection transmit queue binary semaphores with no transmit
*                           permissions by setting the semaphore count to 0.
*                       (2) Initialize TCP connection transmit queue timeout values.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                     TCP/OS initialization successful.
*                               NET_OS_ERR_INIT_TCP_RX_Q            TCP receive  queue(s)         NOT 
*                                                                       successfully initialized.
*                               NET_OS_ERR_INIT_TCP_RX_Q_TIMEOUT    TCP receive  queue timeout(s) NOT 
*                                                                       successfully configured.
*                               NET_OS_ERR_INIT_TCP_TX_Q            TCP transmit queue(s)         NOT 
*                                                                       successfully initialized.
*                               NET_OS_ERR_INIT_TCP_TX_Q_TIMEOUT    TCP transmit queue timeout(s) NOT 
*                                                                       successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_Init (NET_ERR  *perr)
{
    OS_EVENT          **psignal;
    CPU_INT32U          timeout_ms;
    NET_TCP_CONN_QTY    i;
    NET_ERR             net_err;

                                                            /* Initialize TCP connection queues (see Note #1).          */

    psignal    = &NetOS_TCP_RxQ_SemPtr[0];
    timeout_ms = (NET_TCP_CFG_TIMEOUT_CONN_RX_Q_SEC != NET_TMR_TIME_INFINITE  ) ?
                 (NET_TCP_CFG_TIMEOUT_CONN_RX_Q_SEC *  DEF_TIME_NBR_mS_PER_SEC) :
                                                       NET_TMR_TIME_INFINITE;
    for (i = 0; i < NET_TCP_CFG_NBR_CONN; i++) {               
       *psignal = OSSemCreate((INT16U)0);                   /* Create     TCP connection receive queue semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_TCP_RX_Q;
             return;
        }
        psignal++;
                                                            /* Initialize TCP connection receive queue timeout values.  */
        NetOS_TCP_RxQ_TimeoutSet(i, timeout_ms, &net_err);
        if (net_err != NET_TCP_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_TCP_RX_Q_TIMEOUT;
            return;
        }
    }


    psignal    = &NetOS_TCP_TxQ_SemPtr[0];
    timeout_ms = (NET_TCP_CFG_TIMEOUT_CONN_TX_Q_SEC != NET_TMR_TIME_INFINITE  ) ?
                 (NET_TCP_CFG_TIMEOUT_CONN_TX_Q_SEC *  DEF_TIME_NBR_mS_PER_SEC) :
                                                       NET_TMR_TIME_INFINITE;
    for (i = 0; i < NET_TCP_CFG_NBR_CONN; i++) {               
       *psignal = OSSemCreate((INT16U)0);                   /* Create     TCP connection transmit queue semaphores.     */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_TCP_TX_Q;
             return;
        }
        psignal++;
                                                            /* Initialize TCP connection transmit queue timeout values. */
        NetOS_TCP_TxQ_TimeoutSet(i, timeout_ms, &net_err);
        if (net_err != NET_TCP_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_TCP_TX_Q_TIMEOUT;
             return;
        }
    }


   *perr = NET_OS_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_TCP_RxQ_Clr()
*
* Description : Clear TCP connection receive queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to clear receive queue.
*               -----------     Argument validated in NetTCP_RxPktConnHandlerRxQ_AppData(),
*                                                     NetTCP_CloseHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue successfully cleared.
*                               NET_TCP_ERR_RX_Q_CLR            TCP connection receive queue NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxPktConnHandlerRxQ_AppData(),
*               NetTCP_CloseHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Clr (NET_TCP_CONN_ID   conn_id_tcp,
                         NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_TCP_RxQ_SemPtr[conn_id_tcp];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear TCP connection receive queue.                  */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_TCP_ERR_NONE;                       /* ... return NO error.                                 */
             } else {
                *perr = NET_TCP_ERR_RX_Q_CLR;
             }
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_TCP_ERR_RX_Q_CLR;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_TCP_RxQ_Wait()
*
* Description : Wait on TCP connection receive queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to wait on receive queue.
*               -----------     Argument checked in NetTCP_RxAppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue non-empty.
*                               NET_TCP_ERR_RX_Q_EMPTY          TCP connection receive queue still empty by
*                                                                   timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxAppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on TCP connection receive queue forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_TCP_ERR_RX_Q_EMPTY error on TCP connection receive queue 
*                   timeout.  Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Wait (NET_TCP_CONN_ID   conn_id_tcp,
                          NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT16U     os_tick;
    INT8U      os_err;


    psignal = NetOS_TCP_RxQ_SemPtr[conn_id_tcp];
    os_tick = NetOS_TCP_RxQ_TimeoutTbl_tick[conn_id_tcp];
    OSSemPend(psignal, os_tick, &os_err);                       /* Wait on TCP connection receive queue.                */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_TCP_ERR_RX_Q_EMPTY;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TCP_RxQ_Signal()
*
* Description : Signal TCP connection receive queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to signal receive queue.
*               -----------     Argument validated in NetTCP_RxAppData(),
*                                                     NetTCP_RxPktConnHandlerRxQ_AppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue successfully
*                                                                   signaled.
*                               NET_TCP_ERR_RX_Q_FULL           TCP connection receive queue full.
*                               NET_TCP_ERR_RX_Q_SIGNAL         TCP connection receive queue signal failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxAppData(),
*               NetTCP_RxPktConnHandlerRxQ_AppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_Signal (NET_TCP_CONN_ID   conn_id_tcp,
                            NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_TCP_RxQ_SemPtr[conn_id_tcp];
    os_err  = OSSemPost(psignal);                               /* Signal TCP connection receive queue.                 */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_SEM_OVF:
            *perr = NET_TCP_ERR_RX_Q_FULL;
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_TCP_ERR_RX_Q_SIGNAL;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TCP_RxQ_TimeoutSet()
*
* Description : Set TCP connection receive queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to set receive queue timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue timeout
*                                                                   successfully set.
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_TCP_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_RxQ_TimeoutSet (NET_TCP_CONN_ID   conn_id_tcp,
                                CPU_INT32U        timeout_ms,
                                NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (conn_id_tcp < NET_TCP_CONN_ID_MIN) {
       *perr = NET_TCP_ERR_INVALID_CONN;
        return;
    }
    if (conn_id_tcp > NET_TCP_CONN_ID_MAX) {
       *perr = NET_TCP_ERR_INVALID_CONN;
        return;
    }
#endif


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_TCP_RxQ_TimeoutTbl_tick[conn_id_tcp] = os_tick;       /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_TCP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_TCP_RxQ_TimeoutGet_ms()
*
* Description : Get TCP connection receive queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to get receive queue timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection receive queue timeout 
*                                                                   successfully returned.
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*
* Return(s)   : TCP connection receive queue network timeout value (in milliseconds), if NO errors.
*
*               0,                                                                    otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
CPU_INT32U  NetOS_TCP_RxQ_TimeoutGet_ms (NET_TCP_CONN_ID   conn_id_tcp,
                                         NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT32U  timeout_ms;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (conn_id_tcp < NET_TCP_CONN_ID_MIN) {
       *perr =  NET_TCP_ERR_INVALID_CONN;
        return (0);
    }
    if (conn_id_tcp < NET_TCP_CONN_ID_MAX) {
       *perr =  NET_TCP_ERR_INVALID_CONN;
        return (0);
    }
#endif


    CPU_CRITICAL_ENTER();
    os_tick    = NetOS_TCP_RxQ_TimeoutTbl_tick[conn_id_tcp];    /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = NetOS_TimeoutCalc_ms(os_tick);                 /* Calculate   timeout value (in milliseconds).         */

   *perr       = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                         NetOS_TCP_TxQ_Clr()
*
* Description : Clear TCP connection transmit queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to clear transmit queue.
*               -----------     Argument validated in NetTCP_ConnFreeHandler(),
*                                                     NetTCP_TxConnAppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue successfully cleared.
*                               NET_TCP_ERR_TX_Q_CLR            TCP connection transmit queue NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_ConnFreeHandler(),
*               NetTCP_TxConnAppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Clr (NET_TCP_CONN_ID   conn_id_tcp,
                         NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_TCP_TxQ_SemPtr[conn_id_tcp];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear TCP connection transmit queue.                 */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_TCP_ERR_NONE;                       /* ... return NO error.                                 */
             } else {
                *perr = NET_TCP_ERR_TX_Q_CLR;
             }
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_TCP_ERR_TX_Q_CLR;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_TCP_TxQ_Wait()
*
* Description : Wait on TCP connection transmit queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to wait on transmit queue.
*               -----------     Argument checked in NetTCP_TxConnAppData().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue NOT full.
*                               NET_TCP_ERR_TX_Q_FULL           TCP connection transmit queue still full by
*                                                                   timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_TxConnAppData().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on TCP connection transmit queue forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_TCP_ERR_TX_Q_FULL error on TCP connection transmit queue 
*                   timeout.  Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Wait (NET_TCP_CONN_ID   conn_id_tcp,
                          NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT16U     os_tick;
    INT8U      os_err;


    psignal = NetOS_TCP_TxQ_SemPtr[conn_id_tcp];
    os_tick = NetOS_TCP_TxQ_TimeoutTbl_tick[conn_id_tcp];
    OSSemPend(psignal, os_tick, &os_err);                       /* Wait on TCP connection transmit queue.               */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_TCP_ERR_TX_Q_FULL;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TCP_TxQ_Signal()
*
* Description : Signal TCP connection transmit queue.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to signal transmit queue.
*               -----------     Argument validated in NetTCP_RxPktConnHandlerReTxQ().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue successfully
*                                                                   signaled.
*                               NET_TCP_ERR_TX_Q_SIGNAL         TCP connection transmit queue signal failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetTCP_RxPktConnHandlerReTxQ().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_Signal (NET_TCP_CONN_ID   conn_id_tcp,
                            NET_ERR          *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_TCP_TxQ_SemPtr[conn_id_tcp];
    os_err  = OSSemPost(psignal);                               /* Signal TCP connection transmit queue.                */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_TCP_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_TCP_ERR_TX_Q_SIGNAL;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_TCP_TxQ_TimeoutSet()
*
* Description : Set TCP connection transmit queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to set transmit queue timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue timeout
*                                                                   successfully set.
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_TCP_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
void  NetOS_TCP_TxQ_TimeoutSet (NET_TCP_CONN_ID   conn_id_tcp,
                                CPU_INT32U        timeout_ms,
                                NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (conn_id_tcp < NET_TCP_CONN_ID_MIN) {
       *perr = NET_TCP_ERR_INVALID_CONN;
        return;
    }
    if (conn_id_tcp > NET_TCP_CONN_ID_MAX) {
       *perr = NET_TCP_ERR_INVALID_CONN;
        return;
    }
#endif


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_TCP_TxQ_TimeoutTbl_tick[conn_id_tcp] = os_tick;       /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_TCP_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_TCP_TxQ_TimeoutGet_ms()
*
* Description : Get TCP connection transmit queue timeout value.
*
* Argument(s) : conn_id_tcp     Handle identifier of TCP connection to get transmit queue timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_TCP_ERR_NONE                TCP connection transmit queue timeout 
*                                                                   successfully returned.
*                               NET_TCP_ERR_INVALID_CONN        Invalid TCP connection number.
*
* Return(s)   : TCP connection transmit queue network timeout value (in milliseconds), if NO errors.
*
*               0,                                                                     otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_TCP_MODULE_PRESENT
CPU_INT32U  NetOS_TCP_TxQ_TimeoutGet_ms (NET_TCP_CONN_ID   conn_id_tcp,
                                         NET_ERR          *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT32U  timeout_ms;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (conn_id_tcp < NET_TCP_CONN_ID_MIN) {
       *perr =  NET_TCP_ERR_INVALID_CONN;
        return (0);
    }
    if (conn_id_tcp < NET_TCP_CONN_ID_MAX) {
       *perr =  NET_TCP_ERR_INVALID_CONN;
        return (0);
    }
#endif


    CPU_CRITICAL_ENTER();
    os_tick    = NetOS_TCP_TxQ_TimeoutTbl_tick[conn_id_tcp];    /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = NetOS_TimeoutCalc_ms(os_tick);                 /* Calculate   timeout value (in milliseconds).         */

   *perr       = NET_TCP_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                   NETWORK SOCKET LAYER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          NetOS_Sock_Init()
*
* Description : (1) Perform socket/OS initialization :
*
*                   (a) Create socket receive queue semaphores :
*
*                       (1) Initialize socket receive queue semaphores with no received packets by setting 
*                           the semaphore count to 0.
*                       (2) Initialize socket receive queue timeout values.
*
*                   (b) (1) Implement the following socket connection signals ... :
*
*                           (A) Socket connection request
*                           (B) Socket connection accept
*                           (C) Socket connection close
*
*                       (2) ... by creating binary semaphores :
*
*                           (A) Initialize socket connection signals as NOT signaled by setting the binary 
*                               semaphore counts to 0.
*                           (B) Initialize socket connection signal timeout values.
*
*
* Argument(s) : perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                         Socket/OS initialization successful.
*                               NET_OS_ERR_INIT_SOCK_RX_Q               Socket receive queue(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_RX_Q_TIMEOUT       Socket receive queue timeout(s)
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_SOCK_CONN               Socket connection request signal(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_CONN_TIMEOUT       Socket connection request signal timeout(s)
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_SOCK_ACCEPT             Socket connection accept  signal(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_ACCEPT_TIMEOUT     Socket connection accept  signal timeout(s)
*                                                                           NOT successfully configured.
*                               NET_OS_ERR_INIT_SOCK_CLOSE              Socket connection close   signal(s)
*                                                                           NOT successfully initialized.
*                               NET_OS_ERR_INIT_SOCK_CLOSE_TIMEOUT      Socket connection close   signal timeout(s)
*                                                                           NOT successfully configured.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Init().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/
/*$PAGE*/
#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_Init (NET_ERR  *perr)
{
    OS_EVENT      **psignal;
    CPU_INT32U      timeout_ms;
    NET_SOCK_QTY    i;
    NET_ERR         net_err;


                                                                /* Initialize socket receive queues     (see Note #1a). */
    psignal    = &NetOS_Sock_RxQ_SemPtr[0];
    timeout_ms = (NET_SOCK_CFG_TIMEOUT_RX_Q_SEC != NET_TMR_TIME_INFINITE  ) ?
                 (NET_SOCK_CFG_TIMEOUT_RX_Q_SEC *  DEF_TIME_NBR_mS_PER_SEC) :
                                                   NET_TMR_TIME_INFINITE;
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {               
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket receive queue semaphores.          */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_RX_Q;
             return;
        }
        psignal++;
                                                                /* Initialize socket receive queue timeout values.      */
        NetOS_Sock_RxQ_TimeoutSet(i, timeout_ms, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_RX_Q_TIMEOUT;
            return;
        }
    }



/*$PAGE*/
#if (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
                                                                /* Initialize socket connection signals (see Note #1b). */

                                                                /* Initialize socket connection request signal.         */
    psignal    = &NetOS_Sock_ConnReqSignalPtr[0];
    timeout_ms = (NET_SOCK_CFG_TIMEOUT_CONN_REQ_SEC != NET_TMR_TIME_INFINITE  ) ?
                 (NET_SOCK_CFG_TIMEOUT_CONN_REQ_SEC *  DEF_TIME_NBR_mS_PER_SEC) :
                                                       NET_TMR_TIME_INFINITE;
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket connection signal semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_CONN;
             return;
        }
        psignal++;
                                                                /* Initialize socket connection signal timeout values.  */
        NetOS_Sock_ConnReqTimeoutSet(i, timeout_ms, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_CONN_TIMEOUT;
            return;
        }
    }

                                                                /* Initialize socket connection accept  signal.         */
    psignal    = &NetOS_Sock_ConnAcceptSignalPtr[0];
    timeout_ms = (NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_SEC != NET_TMR_TIME_INFINITE  ) ?
                 (NET_SOCK_CFG_TIMEOUT_CONN_ACCEPT_SEC *  DEF_TIME_NBR_mS_PER_SEC) :
                                                          NET_TMR_TIME_INFINITE;
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket connection signal semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_ACCEPT;
             return;
        }
        psignal++;
                                                                /* Initialize socket connection signal timeout values.  */
        NetOS_Sock_ConnAcceptTimeoutSet(i, timeout_ms, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_ACCEPT_TIMEOUT;
            return;
        }
    }

                                                                /* Initialize socket connection close   signal.         */
    psignal    = &NetOS_Sock_ConnCloseSignalPtr[0];
    timeout_ms = (NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_SEC != NET_TMR_TIME_INFINITE  ) ?
                 (NET_SOCK_CFG_TIMEOUT_CONN_CLOSE_SEC *  DEF_TIME_NBR_mS_PER_SEC) :
                                                         NET_TMR_TIME_INFINITE;
    for (i = 0; i < NET_SOCK_CFG_NBR_SOCK; i++) {
       *psignal = OSSemCreate((INT16U)0);                       /* Create     socket connection signal semaphores.      */
        if (*psignal == (OS_EVENT *)0) {
            *perr = NET_OS_ERR_INIT_SOCK_CLOSE;
             return;
        }
        psignal++;
                                                                /* Initialize socket connection signal timeout values.  */
        NetOS_Sock_ConnCloseTimeoutSet(i, timeout_ms, &net_err);
        if (net_err != NET_SOCK_ERR_NONE) {
           *perr = NET_OS_ERR_INIT_SOCK_CLOSE_TIMEOUT;
            return;
        }
    }
#endif


   *perr = NET_OS_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_Sock_RxQ_Clr()
*
* Description : Clear socket receive queue.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear receive queue.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue successfully cleared.
*                               NET_SOCK_ERR_RX_Q_CLR           Socket receive queue NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Clr (NET_SOCK_ID   sock_id,
                          NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_RxQ_SemPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket receive queue.                          */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_RX_Q_CLR;
             }
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_RX_Q_CLR;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                        NetOS_Sock_RxQ_Wait()
*
* Description : Wait on socket receive queue.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on receive queue.
*               -------     Argument checked in NetSock_RxDataHandlerDatagram().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue non-empty.
*                               NET_SOCK_ERR_RX_Q_EMPTY         Socket receive queue still empty by timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_RxDataHandlerDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on socket receive queue forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_SOCK_ERR_RX_Q_EMPTY error on socket receive queue timeout.
*                   Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Wait (NET_SOCK_ID   sock_id,
                           NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     os_tick;
    INT8U      os_err;


    psignal = NetOS_Sock_RxQ_SemPtr[sock_id];
    os_tick = NetOS_Sock_RxQ_TimeoutTbl_tick[sock_id];
    OSSemPend(psignal, os_tick, &os_err);                       /* Wait on socket receive queue.                        */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_RX_Q_EMPTY;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_Sock_RxQ_Signal()
*
* Description : Signal socket receive queue.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal receive queue.
*               -------     Argument validated in NetSock_RxPktDemux(),
*                                                 NetSock_RxDataHandlerDatagram().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue successfully signaled.
*                               NET_SOCK_ERR_RX_Q_FULL          Socket receive queue full.
*                               NET_SOCK_ERR_RX_Q_SIGNAL        Socket receive queue signal failed.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_RxPktDemux(),
*               NetSock_RxDataHandlerDatagram().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_Signal (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_RxQ_SemPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket receive queue.                         */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
            *perr = NET_SOCK_ERR_RX_Q_FULL;
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_RX_Q_SIGNAL;
             break;
    }
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_RxQ_TimeoutSet()
*
* Description : Set socket receive queue timeout value.
*
*               (1) Socket receive queue timeouts apply to the following socket protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set receive queue timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue timeout successfully set.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : --
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Sock_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
void  NetOS_Sock_RxQ_TimeoutSet (NET_SOCK_ID   sock_id,
                                 CPU_INT32U    timeout_ms,
                                 NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
#endif


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_Sock_RxQ_TimeoutTbl_tick[sock_id] = os_tick;          /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_SOCK_ERR_NONE;
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_Sock_RxQ_TimeoutGet_ms()
*
* Description : Get socket receive queue timeout value.
*
*               (1) Socket receive queue timeouts apply to the following socket protocol(s) :
*
*                   (a) Datagram
*                       (1) UDP
*
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get receive queue timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket receive queue timeout 
*                                                                   successfully returned.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
* Return(s)   : Socket receive queue network timeout value (in milliseconds), if NO errors.
*
*               0,                                                            otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
CPU_INT32U  NetOS_Sock_RxQ_TimeoutGet_ms (NET_SOCK_ID   sock_id,
                                          NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT32U  timeout_ms;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
#endif


    CPU_CRITICAL_ENTER();
    os_tick    = NetOS_Sock_RxQ_TimeoutTbl_tick[sock_id];       /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = NetOS_TimeoutCalc_ms(os_tick);                 /* Calculate   timeout value (in milliseconds).         */

   *perr       = NET_SOCK_ERR_NONE;

    return (timeout_ms);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_Sock_ConnReqClr()
*
* Description : Clear socket connection request signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear connection request signal.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal successfully cleared.
*                               NET_SOCK_ERR_CONN_CLR           Socket connection request signal NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqClr (NET_SOCK_ID   sock_id,
                             NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket connection request signal.              */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_CONN_CLR;
             }
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_CONN_CLR;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetOS_Sock_ConnReqWait()
*
* Description : Wait on socket connection request signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on connection request signal.
*               -------     Argument checked in NetSock_ConnHandlerStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection request successfully
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection request NOT signaled
*                                                                       by timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnHandlerStream().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on socket connection request signal forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT error on socket connection
*                   request timeout.  Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqWait (NET_SOCK_ID   sock_id,
                              NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     os_tick;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
    os_tick = NetOS_Sock_ConnReqTimeoutTbl_tick[sock_id];
    OSSemPend(psignal, os_tick, &os_err);                       /* Wait on socket connection request signal.            */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnReqSignal()
*
* Description : Signal socket that connection request complete; socket now connected.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection request complete.
*               -------     Argument checked in NetSock_ConnSignalReq().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection successfully signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL        Socket connection NOT          signaled.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalReq().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqSignal (NET_SOCK_ID   sock_id,
                                NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnReqSignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket connection.                            */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetOS_Sock_ConnReqAbort()
*
* Description : Abort socket connection request.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to abort connection request.
*               -------     Argument checked in NetTCP_ConnCloseApp().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request successfully aborted.
*                               NET_SOCK_ERR_CONN_ABORT    Socket connection request abort failed.
*
* Return(s)   : none.
*
* Caller(s)   : ####
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqAbort (NET_SOCK_ID   sock_id,
                               NET_ERR      *perr)
{
                                                                /* #### Abort feature NOT yet implemented.              */
                                                                /* #### Implement using mailbox/message queue to   ...  */
                                                                /* #### ... pass pointer to static socket octets : ...  */
                                                                /* #### ...     NET_SOCK_CONN_SIGNAL               ...  */
                                                                /* #### ...     NET_SOCK_CONN_ABORT.                    */
   (void)&sock_id;                                              /* Prevent compiler warning.                            */
   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                   NetOS_Sock_ConnReqTimeoutSet()
*
* Description : Set socket connection request signal timeout value.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection request 
*                                   signal timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal timeout
*                                                                   successfully set.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Sock_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnReqTimeoutSet (NET_SOCK_ID   sock_id,
                                    CPU_INT32U    timeout_ms,
                                    NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
#endif


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_Sock_ConnReqTimeoutTbl_tick[sock_id] = os_tick;       /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetOS_Sock_ConnReqTimeoutGet_ms()
*
* Description : Get socket connection request signal timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection request signal
*                               timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection request signal timeout 
*                                                                   successfully returned.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
* Return(s)   : Socket connection request network timeout value (in milliseconds), if NO errors.
*
*               0,                                                                 otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetOS_Sock_ConnReqTimeoutGet_ms (NET_SOCK_ID   sock_id,
                                             NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT32U  timeout_ms;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
#endif


    CPU_CRITICAL_ENTER();
    os_tick    = NetOS_Sock_ConnReqTimeoutTbl_tick[sock_id];    /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = NetOS_TimeoutCalc_ms(os_tick);                 /* Calculate   timeout value (in milliseconds).         */

   *perr       = NET_SOCK_ERR_NONE;

    return (timeout_ms);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnAcceptClr()
*
* Description : Clear socket connection accept signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear connection accept signal.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept signal successfully cleared.
*                               NET_SOCK_ERR_CONN_CLR           Socket connection accept signal NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptClr (NET_SOCK_ID   sock_id,
                                NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnAcceptSignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket connection accept signal.               */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_CONN_CLR;
             }
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_CONN_CLR;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnAcceptWait()
*
* Description : Wait on socket connection accept signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on connection accept signal.
*               -------     Argument checked in NetSock_Accept().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection accept successfully
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection accept NOT signaled
*                                                                       by timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_Accept().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on socket connection accept signal forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT error on socket connection
*                   accept timeout.  Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptWait (NET_SOCK_ID   sock_id,
                                 NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     os_tick;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnAcceptSignalPtr[sock_id];
    os_tick = NetOS_Sock_ConnAcceptTimeoutTbl_tick[sock_id];
    OSSemPend(psignal, os_tick, &os_err);                       /* Wait on socket connection accept signal.             */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_ConnAcceptSignal()
*
* Description : Signal socket that connection request received; socket accept now available.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection accept.
*               -------     Argument checked in NetSock_ConnSignalAccept().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection successfully signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL        Socket connection NOT          signaled.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalAccept().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptSignal (NET_SOCK_ID   sock_id,
                                   NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnAcceptSignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket accept.                                */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetOS_Sock_ConnAcceptTimeoutSet()
*
* Description : Set socket connection accept signal timeout value.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection accept 
*                                   signal timeout.
*
*               timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept signal timeout
*                                                                   successfully set.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Sock_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnAcceptTimeoutSet (NET_SOCK_ID   sock_id,
                                       CPU_INT32U    timeout_ms,
                                       NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
#endif


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_Sock_ConnAcceptTimeoutTbl_tick[sock_id] = os_tick;    /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                NetOS_Sock_ConnAcceptTimeoutGet_ms()
*
* Description : Get socket connection accept signal timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection accept signal
*                               timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection accept signal timeout 
*                                                                   successfully returned.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
* Return(s)   : Socket connection accept network timeout value (in milliseconds), if NO errors.
*
*               0,                                                                otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetOS_Sock_ConnAcceptTimeoutGet_ms (NET_SOCK_ID   sock_id,
                                                NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT32U  timeout_ms;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
#endif


    CPU_CRITICAL_ENTER();
    os_tick    = NetOS_Sock_ConnAcceptTimeoutTbl_tick[sock_id]; /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = NetOS_TimeoutCalc_ms(os_tick);                 /* Calculate   timeout value (in milliseconds).         */

   *perr       = NET_SOCK_ERR_NONE;

    return (timeout_ms);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                      NetOS_Sock_ConnCloseClr()
*
* Description : Clear socket connection close signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to clear connection close signal.
*               -------     Argument validated in NetSock_FreeHandler().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection close signal successfully cleared.
*                               NET_SOCK_ERR_CONN_CLR           Socket connection close signal NOT          cleared.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_FreeHandler().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseClr (NET_SOCK_ID   sock_id,
                               NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     sem_cnt;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
    sem_cnt = 0;
    OSSemSet(psignal, sem_cnt, &os_err);                        /* Clear socket connection close signal.                */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_ERR_TASK_WAITING:                               /* If OS task waiting on semaphore          ...         */
             if (sem_cnt == 0) {                                /* ... but semaphore count cleared to zero, ...         */
                *perr = NET_SOCK_ERR_NONE;                      /* ... return NO error.                                 */
             } else {
                *perr = NET_SOCK_ERR_CONN_CLR;
             }
             break;


        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_CONN_CLR;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                     NetOS_Sock_ConnCloseWait()
*
* Description : Wait on socket connection close signal.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to wait on connection close signal.
*               -------     Argument checked in NetSock_CloseHandlerStream().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE                   Socket connection close successfully
*                                                                       signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT    Socket connection close NOT signaled
*                                                                       by timeout.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_CloseHandlerStream().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : (1) If timeouts NOT desired, wait on socket connection close signal forever (i.e. do NOT exit).
*
*               (2) If timeout desired, return NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT error on socket connection
*                   request timeout.  Implement timeout with OS-dependent function.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseWait (NET_SOCK_ID   sock_id,
                                NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT16U     os_tick;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
    os_tick = NetOS_Sock_ConnCloseTimeoutTbl_tick[sock_id];
    OSSemPend(psignal, os_tick, &os_err);                       /* Wait on socket connection close signal.              */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_TIMEOUT:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        case OS_ERR_PEND_ISR:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL_TIMEOUT;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetOS_Sock_ConnCloseSignal()
*
* Description : Signal socket that connection close complete; socket connection now closed.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to signal connection close complete.
*               -------     Argument checked in NetSock_ConnSignalClose().
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection successfully signaled.
*                               NET_SOCK_ERR_CONN_SIGNAL        Socket connection NOT          signaled.
*
* Return(s)   : none.
*
* Caller(s)   : NetSock_ConnSignalClose().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by 
*               application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseSignal (NET_SOCK_ID   sock_id,
                                  NET_ERR      *perr)
{
    OS_EVENT  *psignal;
    INT8U      os_err;


    psignal = NetOS_Sock_ConnCloseSignalPtr[sock_id];
    os_err  = OSSemPost(psignal);                               /* Signal socket close.                                 */

    switch (os_err) {
        case OS_NO_ERR:
            *perr = NET_SOCK_ERR_NONE;
             break;


        case OS_SEM_OVF:
        case OS_ERR_EVENT_TYPE:
        case OS_ERR_PEVENT_NULL:
        default:
            *perr = NET_SOCK_ERR_CONN_SIGNAL;
             break;
    }
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                  NetOS_Sock_ConnCloseTimeoutSet()
*
* Description : Set socket connection close signal timeout value.
*
* Argument(s) : sock_id         Socket descriptor/handle identifier of socket to set connection close 
*                                   signal timeout.
*
*               timeout_ms      Timeout value (in milliseconds).
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection close signal timeout
*                                                                   successfully set.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
*                                                               - RETURNED BY NetOS_TimeoutCalc_OS_tick() : -
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : none.
*
* Caller(s)   : NetOS_Sock_Init(),
*               Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
void  NetOS_Sock_ConnCloseTimeoutSet (NET_SOCK_ID   sock_id,
                                      CPU_INT32U    timeout_ms,
                                      NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR  cpu_sr;
#endif
    INT16U  os_tick;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr = NET_SOCK_ERR_INVALID_SOCK;
        return;
    }
#endif


    os_tick = NetOS_TimeoutCalc_OS_tick(timeout_ms, perr);      /* Calculate timeout value (in OS ticks).               */
    if (*perr != NET_OS_ERR_NONE) {
         return;
    }

    CPU_CRITICAL_ENTER();
    NetOS_Sock_ConnCloseTimeoutTbl_tick[sock_id] = os_tick;     /* Set OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
    

   *perr = NET_SOCK_ERR_NONE;
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                 NetOS_Sock_ConnCloseTimeoutGet_ms()
*
* Description : Get socket connection close signal timeout value.
*
* Argument(s) : sock_id     Socket descriptor/handle identifier of socket to get connection close signal
*                               timeout.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_SOCK_ERR_NONE               Socket connection close signal timeout 
*                                                                   successfully returned.
*                               NET_SOCK_ERR_INVALID_SOCK       Invalid socket number.
*
* Return(s)   : Socket connection close network timeout value (in milliseconds), if NO errors.
*
*               0,                                                               otherwise.
*
* Caller(s)   : Application.
*
*               This function is a network protocol suite application interface (API) function & MAY be 
*               called by application function(s).
*
* Note(s)     : none.
*********************************************************************************************************
*/

#ifdef  NET_SOCK_MODULE_PRESENT
#if    (NET_SOCK_CFG_TYPE_STREAM_EN == DEF_ENABLED)
CPU_INT32U  NetOS_Sock_ConnCloseTimeoutGet_ms (NET_SOCK_ID   sock_id,
                                               NET_ERR      *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
    CPU_SR      cpu_sr;
#endif
    INT16U      os_tick;
    CPU_INT32U  timeout_ms;


#if (NET_ERR_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)
    if (sock_id < NET_SOCK_ID_MIN) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
    if (sock_id > NET_SOCK_ID_MAX) {
       *perr =  NET_SOCK_ERR_INVALID_SOCK;
        return (0);
    }
#endif


    CPU_CRITICAL_ENTER();
    os_tick    = NetOS_Sock_ConnCloseTimeoutTbl_tick[sock_id];  /* Get OS tick timeout value.                           */
    CPU_CRITICAL_EXIT();
                                                                
    timeout_ms = NetOS_TimeoutCalc_ms(os_tick);                 /* Calculate   timeout value (in milliseconds).         */

   *perr       = NET_SOCK_ERR_NONE;

    return (timeout_ms);
}
#endif
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                               NETWORK/OS TIMEOUT CALCULATION FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     NetOS_TimeoutCalc_OS_tick()
*
* Description : Calculate OS timeout value.
*
* Argument(s) : timeout_ms      Timeout value :
*
*                                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value desired.
*
*                                   In number of milliseconds, otherwise.
*
*               perr        Pointer to variable that will receive the return error code from this function :
*
*                               NET_OS_ERR_NONE                 Timeout successfully set.
*                               NET_OS_ERR_INVALID_TIME         Invalid time value.
*
* Return(s)   : OS timeout value (in OS ticks), if NO errors.
*
*               0,                              otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) (a) uC/OS-II timeout value of '0' implements NO timeout -- i.e. timeouts wait 
*                       forever (see 'uC/OS-II  os_sem.c  OSSemPend()  timeout').
*
*                   (b) (1) Compare 'time_tick' to maximum unsigned value of OS's timeout data type.
*
*                       (2) If an exclusive maximum-value comparison -- i.e. comparison checks for
*                           greater-than-but-not-equal-to -- is used, 'time_tick' MUST be declared 
*                           as an integer data type with a greater resolution -- i.e. greater number 
*                           of bits -- than the OS's timeout data type.
*
*                   (c) To ensure that a non-zero, finite timeout is implemented (see Note #1a), MUST
*                       force at least one OS tick.
*********************************************************************************************************
*/

#ifdef  NET_OS_TIMEOUT_PRESENT
static  INT16U  NetOS_TimeoutCalc_OS_tick (CPU_INT32U   timeout_ms,
                                           NET_ERR     *perr)
{
    CPU_INT32U  time_tick;                                      /* See Note #1b2.                                       */
    INT16U      os_tick;


    if (timeout_ms != NET_TMR_TIME_INFINITE) {                  /* If timeout specified, ...                            */
                                                                /* ... calculate timeout value (in OS ticks).           */
        time_tick = (timeout_ms * OS_TICKS_PER_SEC) / DEF_TIME_NBR_mS_PER_SEC;
        if (time_tick > NET_OS_TIMEOUT_MAX_TICK) {              /* If timeout value > maximum OS tick timeout value ... */
           *perr =  NET_OS_ERR_INVALID_TIME;                    /* ... (see Note #1b), return error.                    */
            return (0);
        }

        if (time_tick > 0) {                                    /* If timeout value > 0, ...                            */
            os_tick = (INT16U)time_tick;                        /* ... cast to OS tick size.                            */
        } else {
            os_tick = (INT16U)1;                                /* Else set at least one OS tick (see Note #1c).        */
        }

    } else {                                                    /* Else for infinite/NO timeout, ...                    */
        os_tick = (INT16U)0;                                    /* ...  set 0 OS ticks (see Note #1a).                  */
    }

   *perr =  NET_OS_ERR_NONE;

    return (os_tick);
}
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                       NetOS_TimeoutCalc_ms()
*
* Description : Get timeout value.
*
* Argument(s) : os_tick     Timeout value (in OS ticks).
*
* Return(s)   : Timeout value :
*
*                   NET_TMR_TIME_INFINITE,     if infinite (i.e. NO timeout) value configured.
*
*                   In number of milliseconds, otherwise.
*
* Caller(s)   : various.
*
* Note(s)     : (1) (a) uC/OS-II timeout value of '0' implements NO timeout -- i.e. timeouts wait 
*                       forever (see 'uC/OS-II  os_sem.c  OSSemPend()  timeout').
*
*                   (b) For infinite uC/OS-II timeout value, return 'NET_TMR_TIME_INFINITE' value.
*********************************************************************************************************
*/

#ifdef  NET_OS_TIMEOUT_PRESENT
static  CPU_INT32U  NetOS_TimeoutCalc_ms (INT16U  os_tick)
{
    CPU_INT32U  timeout_ms;


    if (os_tick == 0) {                                         /* If 0 OS ticks set, ...                               */
        timeout_ms =  (CPU_INT32U)NET_TMR_TIME_INFINITE;        /* ... return infinite/NO timeout value (see Note #1b). */
    } else {                                                    /* Else calculate timeout value (in milliseconds).      */
        timeout_ms = ((CPU_INT32U)os_tick * DEF_TIME_NBR_mS_PER_SEC) / OS_TICKS_PER_SEC;
    }

    return (timeout_ms);
}
#endif

