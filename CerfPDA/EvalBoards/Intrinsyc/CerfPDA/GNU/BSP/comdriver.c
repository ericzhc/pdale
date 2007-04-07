/******************************************************
    Includes
*******************************************************/
#include  "comdriver.h"

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
void SerialDriverInit(short config) 
{
	// Semaphore to protect multiple entry in transmission
	TxSerialSem = OSSemCreate(1);
	TransmitFctSem = OSSemCreate(1);

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

	init_serial_front(config);
}

/*******************************************************
 Called by the interruption service to save the UART's content
*******************************************************/
void ISR_Serial() 
{
	int x;
	switch(GetInterruptStatus())
	{
		case TRANSMIT_INTERRUPT:
			for(x=0; x<TRIGER_LEVEL; x++) {
				if (ptrTxBuffCurr <= ptrTxBuffEnd) {
					THR = TxSerialBuffer[ptrTxBuffCurr];
					ptrTxBuffCurr = (ptrTxBuffCurr + 1) % (int)SERIAL_BUFF_SIZE;
				}
			}
			break;

		case RECEIVER_INTERRUPT:
			for(x=0; x<TRIGER_LEVEL; x++) {
				// Buffer is not full
				if (ptrRxBuffCurr != ptrRxBuffEnd) {
					RxSerialBuffer[ptrRxBuffEnd] = RHR;
					ptrRxBuffEnd = (ptrRxBuffEnd + 1) % (int)SERIAL_BUFF_SIZE;
				}
			}
			break;
	
	}
}

/*******************************************************
 Sends a buffer to the serial port with a maximum size of SERIAL_BUFF_SIZE
*******************************************************/
void TransmitBuffer(char *databuff, int length) 
{
	OSSemPend(TransmitFctSem, 0, &err); // protects dual entry in this fct
	OSSemPend(TxSerialSem, 0, &err);    // protects the transmission buffer
	int i;
	for (i=0; i<length; i++) {
		// Increment the buffer's ending pointer
		ptrTxBuffEnd = ptrTxBuffEnd++ % (int)SERIAL_BUFF_SIZE;
		
		// Sleep until there is an empty buffer spot
		while(ptrTxBuffEnd == ptrTxBuffCurr) {
			OSSemPost(TxSerialSem);
			OSTimeDlyHMSM(0,0,0,10);
			OSSemPend(TxSerialSem, 0, &err);
		}

		TxSerialBuffer[ptrTxBuffEnd] = *(databuff + i);
	}
	OSSemPost(TxSerialSem);
	OSSemPost(TransmitFctSem);
}

/*******************************************************
 Will send any data in the transmission buffer and wait 10 ms 
 between each verification if there's anything to send
*******************************************************/
void BufferTransmissionTask() 
{
	while (1) {
		OSSemPend(TxSerialSem, 0, &err);
		while (ptrTxBuffCurr != ptrTxBuffEnd) {
			ptrTxBuffCurr = ptrTxBuffCurr++ % SERIAL_BUFF_SIZE;
			output_byte_serial_front((char)TxSerialBuffer[ptrTxBuffCurr]);
		}
		OSSemPost(TxSerialSem);
		OSTimeDlyHMSM(0,0,0,10);
	}
}
