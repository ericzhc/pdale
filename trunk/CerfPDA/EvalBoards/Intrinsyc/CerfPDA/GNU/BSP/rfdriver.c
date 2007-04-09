/******************************************************
    Includes
*******************************************************/
#include  <includes.h>

/******************************************************
    Local variables
*******************************************************/
// Transmission Buffers
char TxRfSerialBuffer[SERIAL_BUFF_SIZE];
char RxRfSerialBuffer[SERIAL_BUFF_SIZE];

// Circular buffer pointers
int ptrRfTxBuffCurr = 0, ptrRfTxBuffEnd = 0, ptrRfRxBuffCurr = 0, ptrRfRxBuffEnd = 0;
// Variable to keep track of first init to start buffer task
int firstRfStart = 1;
// Required variables
OS_EVENT* TxRfSerialSem;
OS_EVENT* TransmitRfFctSem;
OS_STK BufferRfTransmissionTaskStk[TASK_RFSERIAL_SIZE];

/*******************************************************
 Needs to be called once from the application integrating this
 driver to initialize the required components
*******************************************************/
void RFDriverInit(short config) 
{
#if DEBUG
	printf("RF driver init\n\r");
#endif
	// Semaphore to protect multiple entry in transmission
	TxRfSerialSem = OSSemCreate(1);
	TransmitRfFctSem = OSSemCreate(1);
	INT8U err;

	// Task that watches the transmit buffer
	if (firstRfStart) {
		OSTaskCreateExt(BufferRfTransmissionTask,
					NULL,
					(OS_STK *)&BufferRfTransmissionTaskStk[TASK_RFSERIAL_SIZE-1],
					TASK_RFSERIAL_PRIO,
					TASK_RFSERIAL_PRIO,
					(OS_STK *)&BufferRfTransmissionTaskStk[0],
					TASK_RFSERIAL_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
		);
		firstRfStart = 0;
	}
	ptrRfRxBuffCurr = 0;
	ptrRfRxBuffEnd = 0;
	ptrRfTxBuffCurr = 0;
	ptrRfTxBuffEnd = 0;

	// Init serial front
	init_serial_front(config);
	setInterruptHandle(ISR_Serial_RF);
	

	printf("RF driver init...done\n\r");
}

/*******************************************************
 Called by the interruption service to save the UART's content
*******************************************************/
void ISR_Serial_RF() 
{

	#if DEBUG
		printf("Interupt\n\r");
	#endif

	int x;
	
	/*switch(GetRFInterruptStatus())
	{
	}*/ 
	
	ICIP = 0; // clear interrupt ?
}

/*******************************************************
 Sends a buffer to the serial port with a maximum size of SERIAL_BUFF_SIZE
*******************************************************/
void TransmitRfBuffer(char *databuff, int length) 
{
	INT8U err;
	OSSemPend(TransmitRfFctSem, 0, &err); // protects dual entry in this fct
	OSSemPend(TxRfSerialSem, 0, &err);    // protects the transmission buffer
	int i;
	for (i=0; i<length; i++) {
		// Increment the buffer's ending pointer
		ptrRfTxBuffEnd = (ptrRfTxBuffEnd+1) % (int)SERIAL_BUFF_SIZE;
		
		// Sleep until there is an empty buffer spot
		while(ptrRfTxBuffEnd == ptrRfTxBuffCurr) {
			OSSemPost(TxRfSerialSem);
			OSTimeDlyHMSM(0,0,0,10);
			OSSemPend(TxRfSerialSem, 0, &err);
		}

		TxRfSerialBuffer[ptrRfTxBuffEnd] = *(databuff + i);
	}
	OSSemPost(TxRfSerialSem);
	OSSemPost(TransmitRfFctSem);
}

/*******************************************************
 Will send any data in the transmission buffer and wait 10 ms 
 between each verification if there's anything to send
*******************************************************/
void BufferRfTransmissionTask() 
{
#if DEBUG
	printf("BufferRfTransmissionTaskStk started\n\r");
#endif
	INT8U err;
	while (1) {
		OSSemPend(TxRfSerialSem, 0, &err);
		while (ptrRfTxBuffCurr != ptrRfTxBuffEnd) {
			ptrRfTxBuffCurr = (ptrRfTxBuffCurr+1) % (int)SERIAL_BUFF_SIZE;
			output_byte_serial_front((char)TxRfSerialBuffer[ptrRfTxBuffCurr]);
		}
		OSSemPost(TxRfSerialSem);
		OSTimeDlyHMSM(0,0,0,10);
	}
}

COM_BUFF_INFO GetTaskRxRfBuff() 
{
	COM_BUFF_INFO buffprot;
	buffprot.Buffer = RxRfSerialBuffer;
	buffprot.ptrCurrent = &ptrRfRxBuffCurr;
	buffprot.ptrEnd = &ptrRfRxBuffEnd;
	
	return buffprot;
}
