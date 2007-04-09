/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2003, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                             CerfPDA SA-1110
*                                          Board Support Package
*
* File    : bsp.c
* By      : Jean J. Labrosse
* Modif by: Philippe Mabilleau, ing., Novembre 2003
* Modif by: Jean-Denis Hatier
* Modif by: Eduardo Luis Romero, Avril 2006
*
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  DEFAULT_STACK_SIZE      512

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

        void*  pExceptAddr;

        OS_STK Stack_User       [DEFAULT_STACK_SIZE];
        OS_STK Stack_FIQ        [DEFAULT_STACK_SIZE];
        OS_STK Stack_IRQ        [DEFAULT_STACK_SIZE];
        OS_STK Stack_Supervisor [DEFAULT_STACK_SIZE];
        OS_STK Stack_Abort      [DEFAULT_STACK_SIZE];
        OS_STK Stack_Undefined  [DEFAULT_STACK_SIZE];
        OS_STK Stack_System     [DEFAULT_STACK_SIZE];

        OS_STK* pStackTop_User          = &Stack_User       [DEFAULT_STACK_SIZE-1];
        OS_STK* pStackTop_FIQ           = &Stack_FIQ        [DEFAULT_STACK_SIZE-1];
        OS_STK* pStackTop_IRQ           = &Stack_IRQ        [DEFAULT_STACK_SIZE-1];
        OS_STK* pStackTop_Supervisor    = &Stack_Supervisor [DEFAULT_STACK_SIZE-1];
        OS_STK* pStackTop_Abort         = &Stack_Abort      [DEFAULT_STACK_SIZE-1];
        OS_STK* pStackTop_Undefined     = &Stack_Undefined  [DEFAULT_STACK_SIZE-1];
        OS_STK* pStackTop_System        = &Stack_System     [DEFAULT_STACK_SIZE-1];

        mmudata memory_mapping[2];
extern  int     int_vector[];
extern  int     mmu_tbl[];


void (*irq_dev_handler_Tab[32])(void);                              /* irq handler tab                                      */
void (*erOnTickHook)           (void);                              /* erOnTickHook function pointer                        */


/*
*********************************************************************************************************
*                                               PROTOTYPES
*********************************************************************************************************
*/

void  BSP_InitIntCtrl();
void  BSP_SetStackPointers();

void  ISR_Reset();
void  ISR_Undefined();
void  ISR_SWI();
void  ISR_CodeAbort();
void  ISR_DataAbort();
void  ISR_IRQ();
void  ISR_FIQ();

void  ISR_Reset_C();
void  ISR_Undefined_C();
void  ISR_SWI_C();
void  ISR_CodeAbort_C();
void  ISR_DataAbort_C();
void  ISR_IRQ_C();
void  ISR_FIQ_C();
void  IRQdefaultHandler(int irq);

void  OSTaskSwHook();
void  Tmr_TickHandler(void);
void  mmu_setup(u32 ttb, u32 dac);
void  exit2iboot(int i);


/*
*********************************************************************************************************
*                                           BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_Init()
{
    ICMR = 0x0L;                                /* Disable ALL interrupts                              */

    memory_mapping[0].v_base    = 0x00000000;
    memory_mapping[0].v_end     = 0xFFEFFFFF;
    memory_mapping[0].p_base    = 0x00000000;
    memory_mapping[0].entrytype = SECTION;
    memory_mapping[0].access    = FULL_ACCESS;
    memory_mapping[0].cb        = BUFFERABLE_AND_CACHEABLE;

    memory_mapping[1].v_base    = 0xFFF00000;
    memory_mapping[1].v_end     = 0xFFFFFFFF;
    memory_mapping[1].p_base    = 0xC3E00000;
    memory_mapping[1].entrytype = SECTION;
    memory_mapping[1].access    = FULL_ACCESS;
    memory_mapping[1].cb        = BUFFERABLE_AND_CACHEABLE;

    int i = build_tbl ((unsigned long) mmu_tbl, memory_mapping, 2, 0, 0);
    mmu_setup ((u32) mmu_tbl, 0x00000001);

    BSP_SetStackPointers();                     /* Initialize the stacks                               */
    erOnTickHook == NULL;                       /* clear "on Tick Hook" registration                   */
    BSP_InitIntCtrl();                          /* Initialize the interrupt controller                 */

    // init_serial (SERIAL_BAUD_38400);
    init_serial (SERIAL_BAUD_115200);
}

/*
*********************************************************************************************************
*                                    INITIALIZE INTERRUPT CONTROLLER
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_InitIntCtrl()
{
    ICMR = 0x0L;                                 /* Disable ALL interrupts                              */

                                                 /* 0xe59ff018L is opcode of (ldr pc,[pc,#0x18])        */
    int_vector[0]  = 0xe59ff018L;                /* Reset exception vector      - jump to 0x0000020    */
    int_vector[1]  = 0xe59ff018L;                /* Undef exception vector      - jump to 0x0000024    */
    int_vector[2]  = 0xe59ff018L;                /* SWI exception vector        - jump to 0x0000028    */
    int_vector[3]  = 0xe59ff018L;                /* Code abort exception vector - jump to 0x000002c    */
    int_vector[4]  = 0xe59ff018L;                /* Data abort exception vector - jump to 0x0000030    */
    int_vector[6]  = 0xe59ff018L;                /* FIQ exception vector        - jump to 0x0000038    */
    int_vector[7]  = 0xe59ff018L;                /* IRQ exception vector        - jump to 0x000003c    */

    int_vector[8]  = (INT32U) ISR_Reset;
    int_vector[9]  = (INT32U) ISR_Undefined;
    int_vector[10] = (INT32U) ISR_SWI;
    int_vector[11] = (INT32U) ISR_CodeAbort;
    int_vector[12] = (INT32U) ISR_DataAbort;
//    int_vector[14] = (INT32U) OS_CPU_Tick_ISR;  /*  modif ELR Avr 2006: commented                     */ 
    int_vector[14] = (INT32U) ISR_IRQ;            /*  modif ELR Avr 2006: added                         */
    int_vector[15] = (INT32U) ISR_FIQ;
}


/*
*********************************************************************************************************
*                                     DISABLE ALL INTERRUPTS
*
* Description : This function disables all interrupts from the interrupt controller.
*               
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    ICMR = 0x0L;                                 /* Disable ALL interrupts                              */
}


/*
*********************************************************************************************************
*                                         EXCEPTION HANDLERS
*
* Description : These function are default exception handlers
*
* Arguments   : none
*********************************************************************************************************
*/

void  ISR_Reset_C()
{
   printf ("\r\nReset exception occured.");
   exit2iboot (-1);
}

void  ISR_Undefined_C()
{
   printf ("\r\nUndefined instruction exception occured at 0x%x.\r\n", pExceptAddr-4);
   exit2iboot (-1);
}

void  ISR_SWI_C()
{
   printf ("\r\nSWI exception occured at 0x%x.\r\n", pExceptAddr-4);
   exit2iboot (-1);
}

void  ISR_CodeAbort_C()
{
   printf ("\r\nCode Abort exception occured at 0x%x.\r\n", pExceptAddr-4);
   exit2iboot (-1);
}

void  ISR_DataAbort_C()
{
   printf ("\r\nData abort exception occured at 0x%x.\r\n", pExceptAddr);
   exit2iboot (-1);
}

/* modif ELR Avr 2006: modified
void  ISR_IRQ_C()
{
   printf ("\r\nIRQ occured at 0x%x.\r\n", pExceptAddr-4);
   exit2iboot (-1);
}
*/

void  ISR_IRQ_C()
{
   printf ("\r\nIRQ occured at 0x%x.\r\n", pExceptAddr-4);
   exit2iboot (-1);
}


/*
*********************************************************************************************************
*                                         IRQ HANDLER MAMAGER
*
* Description : These function allocate and deallocate the handlers for a given interrupt line
*
* Arguments   : none
*********************************************************************************************************
*/

void request_irq(unsigned int irq, void (*handler) (void))
{
    OS_CPU_SR   cpu_sr;

    OS_ENTER_CRITICAL();
    if(handler && irq < 32){                                /* validate the arguments               */
        
		irq_dev_handler_Tab[irq] = handler;                 /* set the handler pointer              */
        ICMR |= 1<<irq;                                     /* set the mask register                */
        ICLR &= ~(1<<irq);                                  /* interrupt routed to IRQ              */
	
    }
    OS_EXIT_CRITICAL();
}

void free_irq(unsigned int irq)
{
    OS_CPU_SR   cpu_sr;

    OS_ENTER_CRITICAL();
    if(irq < 32){                                           /* validate the arguments               */
        irq_dev_handler_Tab[irq] = NULL;                    /* free the handler pointer             */
        ICMR &= ~(1<<irq);                                  /* clear the mask register              */
        ICLR &= ~(1<<irq);
    }
    OS_EXIT_CRITICAL();
}

void IRQdefaultHandler(int irq)
{
    ICMR &= ~(1<<irq);
    ICLR &= ~(1<<irq);
}

void  ISR_FIQ_C()
{
   printf ("\r\nFIQ occured at 0x%x.\r\n", pExceptAddr-4);
   exit2iboot (-1);
}

#if (OS_CPU_HOOKS_EN > 0) && (OS_TASK_SW_HOOK_EN == 0)
void  OSTaskSwHook (void)
{
    OS_STK_DATA TaskStackData;
    INT8U TaskPrio = OSTCBCur->OSTCBPrio;

    OSTaskStkChk (TaskPrio, &TaskStackData);
    if (TaskStackData.OSFree == 0)
    {
        while (1);                              /* Task "TaskPrio" stack overflow                      */
    }
#ifdef OS_VIEW_MODULE
    OSView_TaskSwHook();
#endif
}
#endif


/*
*********************************************************************************************************
*                                         TIMER / TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
*               We decided to use Timer #0 as the tick interrupt source.
*
* Arguments   : none
*
* Notes       :
*********************************************************************************************************
*/

void  Tmr_TickInit(void)
{
                                                /* Initialize the timer to generate 100 Hz             */
    OSSR  = 0x0000000F;                         /* Clear OS Timer ineterrupt channel 0                 */
    OSMR0 = OSCR + CLK_TO_10MS;                 /* Next interrupt in 10 ms                             */
    OSMR0 = OSCR + CLK_TO_1MS;                 /* Next interrupt in 10 ms                             */

    request_irq(26,Tmr_TickHandler);            /* register the irq handler                            */
    OIER  = 0x00000001;                         /* Enable Timer interrupt channel 0                    */
 //   ICMR  = 0x04000000;                         /* Enable OS Timer match register 0 interrupt          */

}


/*
*********************************************************************************************************
* void Tmr_Init(void)
*  This function is provided only for compatibility with the Zoom Card's bsp.c
*
*
*/
void Tmr_Init(void)
{
    Tmr_TickInit();
}


/*
*********************************************************************************************************
*                                          TIMER #0 IRQ HANDLER
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Arguments   : none
*********************************************************************************************************
*/


void  Tmr_TickHandler()
{
    static unsigned int count = 10;
//    OSMR0 += CLK_TO_10MS;                       /* Next interrupt in 10 MS                             */
    OSMR0 = OSCR + CLK_TO_1MS;                 /* Next interrupt in 1 ms                             */
    OSSR   = 0x00000001L;                       /* Clear the tick interrupt source                     */
    OSTimeTick();                               /* If the interrupt is from the tick source, call OSTimeTick() */
    
    if( --count == 0 ){
        if( erOnTickHook )
            erOnTickHook();
        count = 10;
    }
    count &= 0x0f;
}

void onTickHookRegistr( void (*callbk)(void))
{
    erOnTickHook = callbk;
}
