/******************************************************
    Includes
*******************************************************/
#include  <includes.h>
#include  <serial_RF.h>

/******************************************************
    Local variables
*******************************************************/

// Variable to keep track of first init to start buffer task
int firstRfStart = 1;
// Required variables


/*******************************************************
 Needs to be called once from the application integrating this
 driver to initialize the required components
*******************************************************/
void RFDriverInit() 
{
	INT8U err;
#if DEBUG
	printf("RF driver init\n\r");
#endif
	// Semaphore to protect multiple entry in transmission
	//TxRfSerialSem = OSSemCreate(1);
	TransmitRfFctSem = OSSemCreate(1);

	RfFlag = OSFlagCreate(0x00, &err);
	
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


int cell_init()
{
		char buffer[50];
		int err;

		//OSTimeDly(1000);
		
		TransmitRfBuffer("at\n\r\0"); 
		do{
			err = DonneeRecue(buffer,8000);

			if((strstr(buffer,"ERROR") != NULL) || err == 0)
			{
				printf("ERROR -----------------------------------------------");
				return 0;			
			}
		}while(strstr(buffer,"OK")==NULL);	

		TransmitRfBuffer("AT+CGDCONT=1,\"IP\",\"internet.fido.ca\",\"0.0.0.0\",0,0\n\r\0"); 
			do{
			err = DonneeRecue(buffer,8000);

			if((strstr(buffer,"ERROR") != NULL) || err == 0)
			{
				printf("ERROR -----------------------------------------------");
				return 0;			
			}
		}while(strstr(buffer,"OK")==NULL);	

		TransmitRfBuffer("AT#USERID=\"fido\"\n\r\0");
		do{
			err = DonneeRecue(buffer,8000);

			if((strstr(buffer,"ERROR") != NULL) || err == 0)
			{
				printf("ERROR -----------------------------------------------");
				return 0;			
			}
		}while(strstr(buffer,"OK")==NULL);	
		TransmitRfBuffer("AT#PASSW=\"fido\"\n\r\0");
		do{
			err = DonneeRecue(buffer,8000);

			if((strstr(buffer,"ERROR") != NULL) || err == 0)
			{
				printf("ERROR -----------------------------------------------");
				return 0;			
			}
		}while(strstr(buffer,"OK")==NULL);	
		TransmitRfBuffer("AT#SKTSAV\n\r\0");
		do{
			err = DonneeRecue(buffer,8000);
			printf("--------------------------  SKTSAV: %s ",buffer);
			if((strstr(buffer,"ERROR") != NULL) || err == 0)
			{
				printf("ERROR -----------------------------------------------");
				return 0;			
			}
		}while(strstr(buffer,"OK")==NULL);	
		TransmitRfBuffer("AT#GPRS=1\n\r\0");						// Activate GPRS connection (wait for connect)
		do{
			err = DonneeRecue(buffer,8000);

			if((strstr(buffer,"ERROR") != NULL) || err == 0)
			{
				printf("ERROR -----------------------------------------------");
				return 0;			
			}
		}while(strstr(buffer,"+IP")==NULL);	
		

		return 1;
}

int open_socket(char* port,char* ipaddress)
{
	
	char command[100];
	char buffer[50];
	int err;

	sprintf(command,"AT#SKTD=0,%s,\"%s\",0,0\n\r\0",port,ipaddress);
	TransmitRfBuffer(command);
	//TransmitRfBuffer("AT#SKTD=0,2166,\"24.202.172.91\",0,0\n\r\0");
	do{
		DonneeRecue(buffer,8000);
		printf("--------------------- AT#SKTD %s \n\r",	buffer);	

		if((strstr(buffer,"ERROR") != NULL) || err == 0)
		{
			printf("ERROR -----------------------------------------------");
			return 0;			
		}
	}while(strstr(buffer,"CONNECT")==NULL);	

	printf("--------------------------------------- Connected \n\r");

	return 1;
}
	
void close_socket()
{
	TransmitRfBuffer("+++\n\r\0");
}

int DonneeRecue(char* buffer, INT16U timeout)
{
	INT8U err;

	
	memset(buffer,0x00,50);
	// Wait for the data to be received
	
	OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, timeout,&err);
	if(err == OS_NO_ERR)
	{
		if((ptrRfRxBuffCurr != ptrRfRxBuffEnd)) {
			int i = 0;
			//printf("ptrRfRxBuffCurr %d ptrRfRxBuffEnd %d RxRfSerialBuffer[ptrRfRxBuffCurr] %s \n\r ",ptrRfRxBuffCurr,ptrRfRxBuffEnd,RxRfSerialBuffer[ptrRfRxBuffCurr]);
			while((ptrRfRxBuffCurr != ptrRfRxBuffEnd)) {
				//printf("ici2 : %c \n\r",RxRfSerialBuffer[ptrRfRxBuffCurr]);
				ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
				buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
				i++;
			}
			buffer[i]='\0';	

		}

		return 1;
	}
	
	return 0;
}

void checkNetwork()
{
	char buffer[50];

	
	do
	{
		TransmitRfBuffer("AT+CREG\n\r\0");
		//printf("Command send:\n\r");
		if(DonneeRecue(buffer,8000)==0)
			continue;
		
		printf("Data received:%s \n\r", buffer);
		

	}while(strstr(buffer,"+CREG: 0,5")!=NULL);

	//printf("Find Network:");


}


/*******************************************************
 Called by the interruption service to save the UART's content
*******************************************************/
void ISR_Serial_RF() 
{
	unsigned char ucData;
	unsigned char ucErreur;
	unsigned char ucSource;
	INT8U err;
	
	//

	while (1) {
	
	   //erD_sndValHWrdLbl("Hey un interrrupt!",ucSource);
	   // erD_snd_cr();
		ucSource = SERIAL_RF_UTSR0;
		printf("Int source: %x  \n\r",ucSource);
	

															/* Rx is set                                      */
		if ((ucSource & (0x00000002 | 0x00000004)) != 0) {
	
			if (ucSource & 0x00000004) {
				SERIAL_RF_UTSR0 |= 0x00000002;                /* Clear interrupt                                */
			}
			do {
				ucData = SERIAL_RF_UTDR;		              /* Read a char                                  */
				//erD_sndchr(ucData); 
				printf("%c",ucData);
				ptrRfRxBuffEnd = (ptrRfRxBuffEnd + 1) % (int) SERIAL_BUFF_SIZE;
				RxRfSerialBuffer[ptrRfRxBuffEnd] = ucData;	  /* move char into Rx_FIFO                       */
				//printf("%c",RxRfSerialBuffer[ptrRfRxBuffEnd]);
			} while (SERIAL_RF_UTSR1 & 0x00000002);           /* While buffer is not empty                      */

			//OSFlagPost(RfFlag, TCP_TRANSFER_RECEIVED, 0, &err); /* Announce we have received a message          */
			//printf("  \n\r Flag post \n\r");
			OSFlagPost( RfFlag, 
				TCP_TRANSFER_RECEIVED, 
				OS_FLAG_SET, 
				&err);
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
void TransmitRfBuffer(char *databuff)
{

	INT8U err;
	OSSemPend(TransmitRfFctSem, 0, &err); // protects dual entry in this fct
	
	int i;
	for (i=0; databuff[i] != COMMAND_EOL; i++) {
		// Increment the buffer's ending pointer
		ptrRfTxBuffEnd = (ptrRfTxBuffEnd+1) % (int)SERIAL_BUFF_SIZE;
		
		// Sleep until there is an empty buffer spot
		if(ptrRfTxBuffEnd == (ptrRfTxBuffCurr-1)) {
			
			OSFlagPost( RfFlag, 
				TX_RFSERIAL_DATA_READY_TO_SEND, 
				OS_FLAG_SET, 
				&err);

			OSFlagPend(RfFlag, 
				TX_RFSERIAL_DATA_SENT, 
				OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
				0,
				&err);
		}

		TxRfSerialBuffer[ptrRfTxBuffEnd] = databuff[i];
	}
	printf("Flag post\n\r");
	OSFlagPost( RfFlag, 
		TX_RFSERIAL_DATA_READY_TO_SEND, 
		OS_FLAG_SET, 
		&err);

	OSFlagPend(RfFlag, 
		TX_RFSERIAL_DATA_SENT, 
		OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
		0,
		&err);
	
	OSSemPost(TransmitRfFctSem);
}

/*******************************************************
 Will send any data in the transmission buffer and wait 10 ms 
 between each verification if there's anything to send
*******************************************************/
void BufferRfTransmissionTask() 
{
	INT8U err;
	OS_FLAGS flags;

#if DEBUG
	printf("Buffer_Rf_TransmissionTask started\n\r");
#endif
	while (1) {
		flags = OSFlagPend(RfFlag, 
			TX_RFSERIAL_DATA_READY_TO_SEND, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);
		printf("Transmission\n\r\0");
		while (ptrRfTxBuffCurr != ptrRfTxBuffEnd) {
			ptrRfTxBuffCurr = (ptrRfTxBuffCurr+1) % (int)RFSERIAL_BUFF_SIZE;
			output_byte_serial_rf((char)TxRfSerialBuffer[ptrRfTxBuffCurr]);
		}

		OSFlagPost( RfFlag, 
					TX_RFSERIAL_DATA_SENT, 
					OS_FLAG_SET, 
					&err);
	}
}

void boucle()
{
		TransmitRfBuffer("AYE BEGIN TES LACETS SONT DETACHES\n\r\0");
		//printf("Lat: %f - Long: %f - Alt: %f\n\r", GPSPosition.Latitude, GPSPosition.Longitude, GPSPosition.Altitude);
		//printf("Time : %d:%d:%d\n\r", GPSTimeValue.Hours, GPSTimeValue.Minutes, GPSTimeValue.Seconds);
		//TransmitBuffer("testing 123\n\r\0"); 
		//output_byte_serial('a');
		//output_byte_serial('t');
		//output_byte_serial('a');


		OSTimeDly(1000);


}
COM_BUFF_INFO GetTaskRxRfBuff() 
{
	COM_BUFF_INFO buffprot;
	buffprot.Buffer = RxRfSerialBuffer;
	buffprot.ptrCurrent = &ptrRfRxBuffCurr;
	buffprot.ptrEnd = &ptrRfRxBuffEnd;
	
	return buffprot;
}
