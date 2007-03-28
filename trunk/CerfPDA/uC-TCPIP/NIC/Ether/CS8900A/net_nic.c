/* net_nic.c for CS8900 ethernet driver and MPC565 processor
 * Autonomous Solutions, Inc.
 *
 * Hardware reference:
 *   Cirrus Logic CS8900A product Data Sheet
 *   version DS271F2, January 2004.
 */

#define    NET_NIC_MODULE
#include  <net.h>
#include "ASITypes.h"
#include "565Interrupts.h"
#include "CircularBuffer.h"

void        NetNIC_RxISR_Handler     (void);                    /* Rx ISR signals Net Drv Task of NIC rx pkts.  */

void        NetNIC_TxISR_Handler     (void);                    /* Tx ISR signals Net Drv Tx of empty tx FIFO.  */

static  void        NetNIC_TxPktDiscard      (NET_ERR      *perr);


void  NetNIC_Init (void)
{
                                                                /* --------------- PERFORM NIC/OS INIT ---------------- */
    NetOS_NIC_Init();                                           /* Create NIC objs.                                     */

                                                                /* ----------------- INIT NIC STATUS ------------------ */
    NetNIC_ConnStatus           =  DEF_OFF; 


                                                                /* ------------- INIT NIC STAT & ERR CTRS ------------- */
#if (NET_CTR_CFG_STAT_EN        == DEF_ENABLED)
    NetNIC_StatRxPktCtr         =  0;
    NetNIC_StatTxPktCtr         =  0;
#endif

#if (NET_CTR_CFG_ERR_EN         == DEF_ENABLED)
    NetNIC_ErrRxPktDiscardedCtr =  0;
    NetNIC_ErrTxPktDiscardedCtr =  0;
#endif

    CS8900_Init();
}


void  NetNIC_IntEn (void)
{
    CS8900_RxIntEn();
    CS8900_TxIntEn();
}


CPU_BOOLEAN  NetNIC_ConnStatusGet (void)
{
    return (NetNIC_ConnStatus);
}


#define STACK_SIZE_8900_ISR 500
#define RX_BUFFER_STORAGE_SIZE 4096

OS_EVENT * isrSemaphore; 
OS_STK stack8900Isr[STACK_SIZE_8900_ISR]; 
#define  NET_8900_ISR_TASK_PRIO                  (NET_OS_CFG_TASK_PRIO_BASE + 2)
CircularBuffer rxFrameBuffer; 
ASIUint8 rxFrameBufferStorage[RX_BUFFER_STORAGE_SIZE]; 

void  NetNIC_ISR_Handler (void)
{
	//CPU_INT16U int_reg;
	
	// Call NetNIC_RxISR_Handler() or NetNIC_TxISR_Handler() when appropriate.
	// Use the CS8900 PP_ISQ register.
	
	//NetNIC_Wr16(ADD_PORT, PP_ISQ);
	//int_reg = NetNIC_Rd16(DATA_PORT);

	//while(int_reg)
	//{
		/*switch(int_reg & 0x1f)
		{
			case ISQ_RX_EVENT:
			{
				if(int_reg & RX_OK)*/
					OSSemPost(isrSemaphore); 
					//NetNIC_RxISR_Handler();
/*				break; 
			}
			
			case ISQ_TX_EVENT: 
			{
				NetNIC_TxISR_Handler();
				break; 
			}
			
			default: 
			{
			
			}
		}*/
		
	//	NetNIC_Wr16(ADD_PORT, PP_ISQ);
	//	int_reg = NetNIC_Rd16(DATA_PORT);
	//}

	disableIrq(CS8900_INTERRUPT_IRQ_NUMBER); 
#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
    NetNIC_IntClr();                                                /* Clr int ctrl'r int  */
#endif
}


CPU_INT16U  NetNIC_RxPktGetSize (void)
{
    CPU_INT16U  size;


    //size = CS8900_RxPktGetSize();
    CB_Remove(&rxFrameBuffer,(ASIUint8 *)  &size, 2); 

    return (size);
}


void  NetNIC_RxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#if ((NET_CTR_CFG_STAT_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif

    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit rx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

	CB_Remove(&rxFrameBuffer, ppkt, size); 
    //CS8900_RxPkt(ppkt, size);                                /* Rd rx pkt from NIC.                                  */

    //CS8900_RxIntEn();                                        /* See Note #2.                                         */

    NET_CTR_STAT_INC(NetNIC_StatRxPktCtr);

   *perr = NET_NIC_ERR_NONE;
}


void  NetNIC_RxPktDiscard (CPU_INT16U   size,
                           NET_ERR     *perr)
{
#if ((NET_CTR_CFG_ERR_EN      == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit discard (see Note #1).    */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

	CS8900_RxPktDiscard(size);

    NET_CTR_ERR_INC(NetNIC_ErrRxPktDiscardedCtr);

   *perr = NET_NIC_ERR_NONE;
}


void  NetNIC_TxPkt (void        *ppkt,
                    CPU_INT16U   size,
                    NET_ERR     *perr)
{
#ifdef __MWERKS__
	#pragma unused(ppkt)
	#pragma unused(size)
#endif
#if ((NET_CTR_CFG_STAT_EN     == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    if (Net_InitDone != DEF_YES) {                              /* If init NOT complete, exit tx (see Note #1).         */
       *perr = NET_ERR_INIT_INCOMPLETE;
        return;
    }

	CS8900_TxPkt(ppkt, size, perr);

    if (*perr != NET_NIC_ERR_NONE) {
        NetNIC_TxPktDiscard(perr);
        return;
    }

    NET_CTR_STAT_INC(NetNIC_StatTxPktCtr);
}


void  NetNIC_RxISR_Handler (void)
{
    NET_ERR  err;


//DEBUG: Could try disabling the interrupt before sending the signal,
//  unlike the example driver.  Remove this once the OS context switching problem
//  is fixed.
	//CS8900_RxIntDis();
	
    NetOS_IF_RxTaskSignal(&err);

    switch (err) {
        case NET_IF_ERR_NONE:
        	 // DEBUG:  Uncomment this after the OS context switching problem is fixed.
   			 CS8900_RxIntDis();                              /* See Note #2.                                         */
             break;


        case NET_IF_ERR_RX_Q_FULL:
        case NET_IF_ERR_RX_Q_POST_FAIL:
        default:

             NetNIC_RxPktDiscard(0, &err);
             
			 //DEBUG: Enable Rx interrupt again.
			 //  Commenting this out apparently works around (to some extent)
			 //  a major problem with low-level OS internals --
			 //  Our MicroC-OS/II port to the MPC565 is not correctly
			 //  handling the use of semaphore signal posting
			 //  from within ISR routines;  Context switching
			 //  occurs inside an ISR when it shouldn't.
			 //CS8900_RxIntEn();
			 // (Once the OS is fixed, the above line should be uncommented.)
			 
             break;
    }
}


void  NetNIC_TxISR_Handler (void)
{
	CS8900_TxEn();

    NetOS_NIC_TxRdySignal();
}


static  void  NetNIC_TxPktDiscard (NET_ERR  *perr)
{
#if ((NET_CTR_CFG_ERR_EN      == DEF_ENABLED)                    && \
     (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL))
    CPU_SR  cpu_sr;
#endif


    NET_CTR_ERR_INC(NetNIC_ErrTxPktDiscardedCtr);

   *perr = NET_ERR_TX;
}


// CS8900 functions
void cs8900isrTask(void * args); 
void CS8900_BufferFrame(void); 
void cs8900isrTask(void * args)
{
	unsigned char error; 
	unsigned short int_reg; 
	unsigned short rxCount; 
	CPU_INT16U err; 
	(void) args; //eliminate warning
	while(1)
	{
		OSSemPend(isrSemaphore,0,&error); 
		
		rxCount = 0; 
		NetNIC_Wr16(ADD_PORT, PP_ISQ);
		int_reg = NetNIC_Rd16(DATA_PORT);
		while(int_reg)
		{
			switch(int_reg & 0x1f)
			{
				case ISQ_RX_EVENT:
				{
					if(int_reg & RX_OK)
					{
						rxCount++; 
						CS8900_BufferFrame(); 						
					}
					break; 
				}
				
				case ISQ_TX_EVENT: 
				{
					NetNIC_TxISR_Handler();
					break; 
				}
				
				default: 
				{
				
				}
			}
			
			NetNIC_Wr16(ADD_PORT, PP_ISQ);
			int_reg = NetNIC_Rd16(DATA_PORT);
		}
		while(rxCount)
		{
			NetOS_IF_RxTaskSignal(&err); 
			rxCount--; 
		}	
		enableIrq(CS8900_INTERRUPT_IRQ_NUMBER); 	
	}
}

ASIUint8 currentFrame[1024]; 

void CS8900_BufferFrame(void)
{
	ASIUint16 size; 

	//Insert frame size, then frame data. 
	size = CS8900_RxPktGetSize(); 
	CB_Insert(&rxFrameBuffer,(ASIUint8 *)  &size, 2);
	
	CS8900_RxPkt(currentFrame, size); 
	
	CB_Insert(&rxFrameBuffer, currentFrame, size); 	
	 
	
}

void CS8900_Init(void)
{
    CPU_INT16U   reg_val;
    CPU_INT08U   i;
    CPU_SR 		 cpu_sr;
    
   	CS8900_PwrCtrl(DEF_ON);
	CS8900_WaitForReset();

 	NetNIC_Wr16(ADD_PORT, PP_LineCTL);
    NetNIC_Wr16(DATA_PORT, SERIAL_RX_ON | SERIAL_TX_ON);

 	NetNIC_Wr16(ADD_PORT, PP_RxCTL);
    NetNIC_Wr16(DATA_PORT, RX_OK_ACCEPT | RX_IA_ACCEPT | RX_BROADCAST_ACCEPT);	
    //NetNIC_Wr16(DATA_PORT, RX_PROM_ACCEPT);	
	
	CPU_CRITICAL_ENTER();
#if (CS8900_CFG_MAC_ADDR_SEL == CS8900_MAC_ADDR_SEL_EEPROM)
	// Get MAC address from CS8900 Individual Address register.
	// Index 0 is most significant byte.
	for (i = 0; i < sizeof(NetIF_MAC_Addr); i += sizeof(CPU_INT16U))
	{
		NetNIC_Wr16(ADD_PORT, PP_IA + i);
		reg_val = NetNIC_Rd16(DATA_PORT);

        NetIF_MAC_Addr[i    ] = (CPU_INT08U) reg_val;
        NetIF_MAC_Addr[i + 1] = (CPU_INT08U)(reg_val  >> DEF_OCTET_NBR_BITS);
    }
#endif
	CPU_CRITICAL_EXIT();
	
	// TO DO: Could set this variable based on link status events.
    NetNIC_ConnStatus = DEF_ON;
    
    isrSemaphore = OSSemCreate(0); 
    CB_InitializeBuffer(&rxFrameBuffer, rxFrameBufferStorage, RX_BUFFER_STORAGE_SIZE,(ASIUint8 *) "CS8900 RX", 1); 
    OSTaskCreate (cs8900isrTask,	NULL, (void *)&stack8900Isr[STACK_SIZE_8900_ISR-2], NET_8900_ISR_TASK_PRIO);
	
#if (NET_NIC_CFG_INT_CTRL_EN == DEF_ENABLED)
	NetNIC_IntInit();
#endif
}


void CS8900_Shutdown(void)
{
   	CS8900_PwrCtrl(DEF_OFF);
}


void CS8900_PwrCtrl(CPU_BOOLEAN pwr)
{
    if (pwr == DEF_ON) {
 	    NetNIC_Wr16(ADD_PORT, PP_SelfCTL);
        NetNIC_Wr16(DATA_PORT, POWER_ON_RESET);
    } else {
 	    NetNIC_Wr16(ADD_PORT, PP_SelfCTL);
        NetNIC_Wr16(DATA_PORT, SW_STOP);
    }
}


void CS8900_WaitForReset(void)
{
	unsigned short i;

	NetNIC_Wr16(ADD_PORT, PP_SelfST);
	i = NetNIC_Rd16(DATA_PORT);
	while (!(i & INIT_DONE))
	{
		i = NetNIC_Rd16(DATA_PORT);
	}  // Wait until chip reset is done.
}


void CS8900_RxIntDis(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif

	CPU_CRITICAL_ENTER();
	
	NetNIC_Wr16(ADD_PORT, PP_RxCFG);
    NetNIC_Wr16(DATA_PORT, 0);

    CPU_CRITICAL_EXIT();
}


void CS8900_RxIntEn(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif

	CPU_CRITICAL_ENTER();
	
	NetNIC_Wr16(ADD_PORT, PP_RxCFG);
    NetNIC_Wr16(DATA_PORT, RX_OK_ENBL);

    CPU_CRITICAL_EXIT();
}


static void CS8900_RxPkt(void *ppkt, CPU_INT16U sizeParameter)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif
  unsigned short size = sizeParameter;
  
  CPU_CRITICAL_ENTER();
	
  while (size > 1) {
  *((unsigned short *)ppkt)++ = CS8900_ReadFrame();
    size -= 2;
  }
  
  if (size)                                      // check for leftover byte...
  {
  		*(unsigned char *)ppkt = (CS8900_ReadFrame() >>8);
  }
  
  CPU_CRITICAL_EXIT();
}

unsigned short CS8900_ReadFrame(void)
{
	return *((unsigned short *) (CS8900IOBASE + (RX_FRAME_PORT<<11)));  
}


static void CS8900_RxPktDiscard(CPU_INT16U size)
{
#ifdef __MWERKS__
	#pragma unused(size)
#endif
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif

	CPU_CRITICAL_ENTER();
	
	NetNIC_Wr16(ADD_PORT, PP_RxCFG);
    NetNIC_Wr16(DATA_PORT, SKIP_1);	

    CPU_CRITICAL_EXIT();
	
	// DEBUG:  Uncomment this once the OS context switch problem is fixed.
	CS8900_RxIntEn();	
}


static void CS8900_TxEn(void)
{
	// TO DO: To be implemented.  Is there anything to do here?
}


void CS8900_TxIntEn(void)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif

	CPU_CRITICAL_ENTER();
	
	NetNIC_Wr16(ADD_PORT, PP_TxCFG);
    NetNIC_Wr16(DATA_PORT, TX_OK_ENBL);

	NetNIC_Wr16(ADD_PORT, PP_BufCFG);
    NetNIC_Wr16(DATA_PORT, READY_FOR_TX_ENBL);
    
    CPU_CRITICAL_EXIT();
}


void CS8900_WriteFrame(unsigned short Data)
{
	*((unsigned short *) (CS8900IOBASE + (TX_FRAME_PORT<<CS8900_ADDR_SHIFT) )) = Data;
}

void CS8900_CopyToFrame(void *Source, unsigned short sizeParameter)
{
  unsigned short Size = sizeParameter;
  
  while (Size > 1) {
    CS8900_WriteFrame(*((unsigned short *)Source)++);
    Size -= 2;
  }
  
  if (Size)
  {
  	unsigned short value;
  	value = (*(unsigned char *)Source) << 8;
  	CS8900_WriteFrame(value);
  }
}


static void CS8900_TxPkt(void *ppkt, CPU_INT16U size, NET_ERR *perr)
{
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif
	CPU_INT16U status;

	CPU_CRITICAL_ENTER();
	
	// Transmit a packet, as described in section
	// 5.6.7  Transmit in Interrupt Mode
	// of the Cirrus Logic CS8900A product Data Sheet
	// version DS271F2, January 2004.
	
	// "Bid" for frame storage by writing a
	// transmit command.
	NetNIC_Wr16(ADD_PORT, PP_TxCommand);
    NetNIC_Wr16(DATA_PORT, TX_START_ALL_BYTES);

	// Write transmit frame length.
	NetNIC_Wr16(ADD_PORT, PP_TxLength);
    NetNIC_Wr16(DATA_PORT, size);
    
    // Read bus status register.
    NetNIC_Wr16(ADD_PORT, PP_BusST);
	status = NetNIC_Rd16(DATA_PORT);
	
    CPU_CRITICAL_EXIT();


	// If not ready to transmit, return.
	if (!(status & READY_FOR_TX_NOW))
	{
	    *perr = NET_ERR_TX;
        return;
	}

	CS8900_CopyToFrame(ppkt, size);
	

	CPU_CRITICAL_ENTER();

	// Set the TxOK bit to indicate completion of
	// frame write.
	NetNIC_Wr16(ADD_PORT, PP_TxEvent);
    NetNIC_Wr16(DATA_PORT, TX_OK);	
	
    CPU_CRITICAL_EXIT();


   *perr = NET_NIC_ERR_NONE;

}


static CPU_INT16U CS8900_RxPktGetSize(void)
{
	CPU_INT16U size;
#if (CPU_CFG_CRITICAL_METHOD == CPU_CRITICAL_METHOD_STATUS_LOCAL)
	CPU_SR      cpu_sr;
#endif
  
    CPU_CRITICAL_ENTER();
	
	NetNIC_Wr16(ADD_PORT, PP_RxLength);
	size = NetNIC_Rd16(DATA_PORT);
	
    CPU_CRITICAL_EXIT();

	return(size);
}


void NetNIC_ISR (unsigned char Level)
{
#ifdef __MWERKS__
	#pragma unused(Level)
#endif
	NetNIC_ISR_Handler();
}

void  NetNIC_IntInit (void)
{
	NetNIC_Wr16(ADD_PORT, PP_CS8900_ISAINT);
    NetNIC_Wr16(DATA_PORT, 0);
    
    // Disable interrupts now;
    // Interrupts are enabled in the functions
    // CS8900_RxIntEn() and CS8900_TxIntEn().

    NetNIC_Wr16(ADD_PORT, PP_RxCFG);
    NetNIC_Wr16(DATA_PORT, 0);

	NetNIC_Wr16(ADD_PORT, PP_TxCFG);
    NetNIC_Wr16(DATA_PORT, 0);
    
	registerIrqHandler(NetNIC_ISR, CS8900_INTERRUPT_IRQ_NUMBER);

 	NetNIC_Wr16(ADD_PORT, PP_BusCTL);
    NetNIC_Wr16(DATA_PORT, ENABLE_IRQ);   
}

void  NetNIC_IntClr (void)
{
	// Current thinking is that nothing needs to be done
	// here;  Perhaps the pending interrupt is cleared
	// when it is read by the OS.
}

CPU_INT16U  NetNIC_Rd16 (CPU_INT16U reg_offset)
{
	unsigned short *p;
	unsigned short value;
	unsigned short swappedValue;
	
	p = (unsigned short *)(CS8900IOBASE + (((unsigned int) reg_offset)<<11));
	
	value = *((unsigned short *) p);
	swappedValue = NetSwapBytes(value);

	return swappedValue;
}

void  NetNIC_Wr16 (CPU_INT16U reg_offset, CPU_INT16U val)
{
	unsigned short *p;
	unsigned short swappedValue;
	
	swappedValue = NetSwapBytes(val);
	p = (unsigned short *)(CS8900IOBASE + (((unsigned int) reg_offset)<<CS8900_ADDR_SHIFT));
	*((unsigned short *) p) = swappedValue;
}

NET_TS  NetUtil_TS_Get (void)
{
    /* $$$$ Insert code to return Internet Timestamp.   */

    return ((NET_TS)0);
}

void  NetTCP_InitTxSeqNbr (void)
{
}


unsigned short NetSwapBytes(unsigned short Data)
{
  return (Data >> 8) | (Data << 8);
}

