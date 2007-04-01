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

// Required variables
OS_EVENT* TxSem;
OS_EVENT* RxSerialSem;
OS_EVENT* TransmitFctSem;
OS_STK BufferTransmissionTaskStk[TASK_SERIAL_SIZE];
INT8U err;

/*******************************************************
 Needs to be called once from the application integrating this
 driver to initialize the required components
*******************************************************/
void SerialDriverInit() 
{
	// Semaphore to protect multiple entry in transmission
	TxSem = OSSemCreate(1);
	RxSerialSem	= OSSemCreate(1);
	TransmitFctSem = OSSemCreate(1);

	// Task that watches the transmit buffer
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
}

/*******************************************************
 Initalizes the UART
*******************************************************/
void ComInit(u32 baud) 
{
	free_irq(COM_PORT);
	request_irq(COM_PORT, ISR_Serial);
	init_serial_front(baud);
	// Clear serial buffers
	ptrRxBuffCurr = 0;
	ptrRxBuffEnd = 0;
	ptrTxBuffCurr = 0;
	ptrTxBuffEnd = 0;
}

/*******************************************************
 Called by the interruption service to save the UART's content
*******************************************************/
void ISR_Serial() 
{
	OSSemPend(RxSerialSem, 0, &err);
	OSSemPost(RxSerialSem);
}

/*******************************************************
 Sends a buffer to the serial port with a maximum size of SERIAL_BUFF_SIZE
*******************************************************/
void TransmitBuffer(char *databuff, int length) 
{
	OSSemPend(TransmitFctSem, 0, &err); // protects dual entry in this fct
	OSSemPend(TxSem, 0, &err); // protects the transmission buffer
	int i;
	for (i=0; i<length; i++) {
		// Increment the buffer's ending pointer
		ptrTxBuffEnd = ptrTxBuffEnd++ % SERIAL_BUFF_SIZE;
		
		// Sleep until there is an empty buffer spot
		while(ptrTxBuffEnd == ptrTxBuffCurr) {
			OSSemPost(TxSem);
			OSTimeDlyHMSM(0,0,0,10);
			OSSemPend(TxSem, 0, &err);
		}

		TxSerialBuffer[ptrTxBuffEnd] = (databuff + i)[0];
	}
	OSSemPost(TxSem);
	OSSemPost(TransmitFctSem);
}

/*******************************************************
 Will send any data in the transmission buffer and wait 10 ms 
 between each verification if there's anything to send
*******************************************************/
void BufferTransmissionTask() 
{
	while (1) {
		OSSemPend(TxSem, 0, &err);
		while (ptrTxBuffCurr != ptrTxBuffEnd) {
			ptrTxBuffCurr = ptrTxBuffCurr++ % SERIAL_BUFF_SIZE;
			output_byte_serial_front((char)TxSerialBuffer[ptrTxBuffCurr]);
		}
		OSSemPost(TxSem);
		OSTimeDlyHMSM(0,0,0,10);
	}
}

TASK_BUFF_PROTECT GetTaskBuffProtectStruct() 
{
	OSSemPend(RxSerialSem, 0, &err);
	
	TASK_BUFF_PROTECT buffprot;
	buffprot.RxBuffer = RxSerialBuffer;
	buffprot.semaphore = RxSerialSem;
	buffprot.ptrCurrent = &ptrRxBuffCurr;
	buffprot.ptrEnd = &ptrRxBuffEnd;
	
	OSSemPost(RxSerialSem);

	return buffprot;
}
