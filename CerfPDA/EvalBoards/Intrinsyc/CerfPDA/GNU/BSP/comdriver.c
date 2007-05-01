/******************************************************
    Includes
*******************************************************/
#include  <includes.h>

/******************************************************
    Local variables
*******************************************************/
// Transmission Buffers
char TxSerialBuffer[SERIAL_BUFF_SIZE];
char RxSerialBuffer[SERIAL_BUFF_SIZE];

// Circular buffer pointers
int ptrTxBuffCurr = 0, ptrTxBuffEnd = 0, ptrRxBuffCurr = 0, ptrRxBuffEnd = 0;
// Variable to keep track of first init to start buffer task
int firstStart = 1;
// Required variables
OS_EVENT* TxSerialSem;
OS_EVENT* TransmitFctSem;
OS_STK BufferTransmissionTaskStk[TASK_SERIAL_SIZE];
INT8U err;

/*******************************************************
 Needs to be called once from the application integrating this
 driver to initialize the required components
*******************************************************/
void ComDriverInit(short config) 
{
#if DEBUG
	printf("Com driver init\n\r");
#endif
	// Semaphore to protect multiple entry in transmission
	TxSerialSem = OSSemCreate(1);
	TransmitFctSem = OSSemCreate(1);
	/* Initialisation du flag de communication */
	comFlag = OSFlagCreate(0x00, &err);

	// Task that watches the transmit buffer
	if (firstStart) {
		OSTaskCreateExt(BufferTransmissionTask,
					NULL,
					(OS_STK *)&BufferTransmissionTaskStk[TASK_SERIAL_SIZE-1],
					TASK_SERIAL_PRIO,
					TASK_SERIAL_PRIO,
					(OS_STK *)&BufferTransmissionTaskStk[0],
					TASK_SERIAL_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
		);
		firstStart = 0;
	}
	ptrRxBuffCurr = 0;
	ptrRxBuffEnd = 0;
	ptrTxBuffCurr = 0;
	ptrTxBuffEnd = 0;

	// Init serial front
	setInterruptHandle(ISR_Serial);
	init_serial_front(config);
	
	

	printf("Com driver init...done\n\r");
}

/*******************************************************
 Called by the interruption service to save the UART's content
*******************************************************/
void ISR_Serial() 
{
	#if DEBUG
		//printf("Interupt IIR: %2.2x \n\r",IIR);
	#endif

	int x;
	int trash;

	switch(GetInterruptStatus())
	{
		case TRANSMIT_INTERRUPT:
			while(!txFIFOEmpty()) {
				if (ptrTxBuffCurr <= ptrTxBuffEnd) {
					THR = TxSerialBuffer[ptrTxBuffCurr];
					ptrTxBuffCurr = (ptrTxBuffCurr + 1) % (int)SERIAL_BUFF_SIZE;
				}
			}
			break;
		case RECEIVER_INTERRUPT: 
			
			while(rxfifoFull()) {
				//trash = RHR;
				ptrRxBuffEnd = (ptrRxBuffEnd + 1) % (int)SERIAL_BUFF_SIZE;
				
				// Buffer is not full
				if (ptrRxBuffCurr != ptrRxBuffEnd) {
					RxSerialBuffer[ptrRxBuffEnd] = RHR;
					
				#if DEBUG
						//printf("Received car %c \n\r",RxSerialBuffer[ptrRxBuffEnd]);
				#endif
					
				} else {
					break;
				}
			}
			OSFlagPost(comFlag, 
				RX_SERIAL_DATA_AVAILABLE, 
				OS_FLAG_SET, 
				&err);
			break;
		default:
			#if DEBUG
				printf("Unsupported interupt\n\r");
			#endif
			break;
	} 
	
	GEDR_SF = 0x08;				// clear GEDR.3 
	GPCR_SF = 0x08;	
	x = MSR;
}

/*******************************************************
 Sends a buffer to the serial port with a maximum size of SERIAL_BUFF_SIZE
*******************************************************/
void TransmitBuffer(char *databuff) 
{
	OS_FLAGS flags;
	OSSemPend(TransmitFctSem, 0, &err); // protects dual entry in this fct

	int i;
	for (i=0; databuff[i] != '\0'; i++) {
		// Increment the buffer's ending pointer
		ptrTxBuffEnd = (ptrTxBuffEnd+1) % (int)SERIAL_BUFF_SIZE;
		
		// Sleep until there is an empty buffer spot
		if (ptrTxBuffEnd == (ptrTxBuffCurr-1)) {
			OSFlagPost( comFlag, 
				TX_SERIAL_DATA_READY_TO_SEND, 
				OS_FLAG_SET, 
				&err);

			flags = OSFlagPend(comFlag, 
				TX_SERIAL_DATA_SENT, 
				OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
				0,
				&err);
		}

		TxSerialBuffer[ptrTxBuffEnd] = databuff[i];
	}
	OSFlagPost( comFlag, 
		TX_SERIAL_DATA_READY_TO_SEND, 
		OS_FLAG_SET, 
		&err);

	flags = OSFlagPend(comFlag, 
		TX_SERIAL_DATA_SENT, 
		OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
		0,
		&err);

	OSSemPost(TransmitFctSem);
}

/*******************************************************
 Will send any data in the transmission buffer and wait 10 ms 
 between each verification if there's anything to send
*******************************************************/
void BufferTransmissionTask() 
{
	OS_FLAGS flags;

#if DEBUG
	printf("BufferTransmissionTask started\n\r");
#endif
	while (1) {
		flags = OSFlagPend(comFlag, 
			TX_SERIAL_DATA_READY_TO_SEND, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);

		while (ptrTxBuffCurr != ptrTxBuffEnd) {
			ptrTxBuffCurr = (ptrTxBuffCurr+1) % (int)SERIAL_BUFF_SIZE;
			output_byte_serial_front(TxSerialBuffer[ptrTxBuffCurr]);
		}

		OSFlagPost( comFlag, 
					TX_SERIAL_DATA_SENT, 
					OS_FLAG_SET, 
					&err);
	}
}
/*******************************************************
 Used to get informations about the serial Buffer
*******************************************************/
COM_BUFF_INFO GetTaskRxComBuff() 
{
	COM_BUFF_INFO buffprot;
	buffprot.Buffer = RxSerialBuffer;
	buffprot.ptrCurrent = &ptrRxBuffCurr;
	buffprot.ptrEnd = &ptrRxBuffEnd;
	
	return buffprot;
}
