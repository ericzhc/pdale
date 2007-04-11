/******************************************************
    Includes
*******************************************************/
#include  <includes.h>
#include  <serial_RF.h>

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
void RFDriverInit() 
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

	// Init serial rf
	setInterruptHandle_rf(ISR_Serial_RF);
	init_serial_rf(SERIAL_BAUD_115200);
	
	

	printf("RF driver init...done\n\r");
}


void cell_init()
{
		OSTimeDly(1000);
		TransmitRfBuffer("at\n\r", 4); 
		OSTimeDly(1000);
		//TransmitRfBuffer("at+cmee=1\n\r", 10); 
		//OSTimeDly(2000);
		TransmitRfBuffer("AT+CGDCONT=1,\"IP\",\"internet.fido.ca\",\"0.0.0.0\",0,0\n\r", 52); 
		OSTimeDly(1000);
		TransmitRfBuffer("AT#USERID=\"fido\"\n\r",18);
		OSTimeDly(1000);
		TransmitRfBuffer("AT#PASSW=\"fido\"\n\r",17);
		OSTimeDly(1000);
		TransmitRfBuffer("AT#SKTSAV\n\r",11);
		OSTimeDly(1000);
		TransmitRfBuffer("AT#GPRS=1\n\r",12);						// Activate GPRS connection (wait for connect)
		OSTimeDly(1000);

}

void open_socket(char* port,char* ipaddress)
{
	OSTimeDly(2000);
	char command[400];

	sprintf(command,"AT#SKTD=0,%s,\"%s\",0,0\n\r",port,ipaddress);
	TransmitRfBuffer(command,strlen(command));
	
	OSTimeDly(8000);
	OSTimeDly(8000);
}

void close_socket()
{
	TransmitRfBuffer("+++\n\r",5);
}
/*******************************************************
 Called by the interruption service to save the UART's content
*******************************************************/
void ISR_Serial_RF() 
{

	unsigned char ucData;
	unsigned char ucErreur;
	unsigned char ucSource;

	while (1) {
		ucSource = SERIAL_RF_UTSR0;
		//erD_sndValHWrdLbl("Hey un interrrupt!",ucSource);
	   // erD_snd_cr();
		                                                        /* Rx is set                                      */
		if (ucSource & (0x00000002 | 0x00000004)) {
	
			if (ucSource & 0x00000004) {
				SERIAL_RF_UTSR0 |= 0x00000002;                /* Clear interrupt                                */
			}
			do {
				ucData = SERIAL_RF_UTDR;		                /* Read a char                                  */
				erD_sndchr(ucData);                                                
				                                                /* move char into Rx_FIFO                       */
				
			} while (SERIAL_RF_UTSR1 & 0x00000002);           /* While buffer is not empty                      */
			continue;
		}
                                                                /* Rbb is set                                     */
		if (ucSource & 0x00000008) {
			SERIAL_RF_UTSR0 |= 0x00000008;                    /* Clear interrupt                                */
			continue;
		}
		                                                        /* Reb is set                                     */
		if (ucSource & 0x00000010) {
			SERIAL_RF_UTSR0 |= 0x00000010;                    /* Clear interrupt                                */
			continue;
		}
                                                                /* Eif is set                                     */
		if (ucSource & 0x00000020) {
			do {
				ucErreur = SERIAL_RF_UTSR1;
				ucData = SERIAL_RF_UTDR;
			} while (SERIAL_RF_UTSR0 & 0x00000020);
			continue;
		}
		break;
	}
	
	 SERIAL_UTSR0 = 0xff;
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
			output_byte_serial_rf((char)TxRfSerialBuffer[ptrRfTxBuffCurr]);
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
