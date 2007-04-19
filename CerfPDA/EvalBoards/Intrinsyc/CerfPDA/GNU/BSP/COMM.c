/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Methods
*******************************************************/

void ReceiveData(char cmd, char* buffer)
{
	INT8U err = OS_NO_ERR;
	//OSSemPend(TransmitRfFctSem, 0, &err); // protects dual entry in this fct
	//OSSemPend(ReceiveDataSem, 0, &err); // protects dual entry in this fct

	
	// Wait for the data to be received
	int i = 0;
	int timeout = 0;
	//erD_sndstr("Wait for the data to be received");
	OSTimeDlyHMSM(0,0,2,0);
	ptrRfRxBuffCurr = ptrRfRxBuffEnd = 0;
	while (err != OS_TIMEOUT) {
		OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, timeout,&err);
		timeout = 1000;

		if(err == OS_NO_ERR)
		{
			while(ptrRfRxBuffCurr != ptrRfRxBuffEnd) {
				ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
				buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
				//erD_sndchr(RxRfSerialBuffer[ptrRfRxBuffCurr]);
				i++;
			}
		}
	}
	buffer[i] = '\0';
	//erD_sndstr("Messages recus : ");
	//erD_sndstr(buffer);
	//OSSemPost(ReceiveDataSem);
	//OSSemPost(TransmitRfFctSem);
}

void GetTruckNames(char* buffer)
{
	INT8U err = OS_NO_ERR;
	char data[] = {COMMAND_TRUCKNAMES, ';', COMMAND_EOL};

	// Send command
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_TRUCKNAMES, buffer);
	OSSemPost(ReceiveDataSem);
}

char IsValidPackage(char* packetid)
{
	int i;
	INT8U err = OS_NO_ERR;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_VALIDPACKAGE;

	for(i=0; packetid[i] != COMMAND_EOL; i++) {
		data[i+1] = packetid[i];
	}

	data[i+1] = ';';
	data[i+2] = COMMAND_EOL;

	// Send message
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);

	//char data[] = {COMMAND_VALIDPACKAGE, packetid, ';', COMMAND_EOL};
	char tempbuff[2];

	// Wait for response
	ReceiveData(COMMAND_VALIDPACKAGE, tempbuff);
	OSSemPost(ReceiveDataSem);
	return tempbuff[0];
}

void GetPacketInfos(char* packetid, char* buffer)
{
	int i;
	INT8U err = OS_NO_ERR;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_PACKETINFOS;

	for(i=0; packetid[i] != COMMAND_EOL; i++) {
		data[i+1] = packetid[i];
	}

	data[i+1] = ';';
	data[i+2] = COMMAND_EOL;

	// Send message
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);

	// Wait for response
	ReceiveData(COMMAND_PACKETINFOS, buffer);
	OSSemPost(ReceiveDataSem);
}

void SetPacketState(char* packetid, char value)
{
	int i;
	INT8U err = OS_NO_ERR;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_SETPACKETSTATE;
	data[1] = value;
	for(i=0; packetid[i] != COMMAND_EOL; i++) {
		data[i+2] = packetid[i];
	}

	data[i+2] = ';';
	data[i+3] = COMMAND_EOL;

	// Send command
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);
	OSSemPost(ReceiveDataSem);
}

void GetAllPackages(int truckid, char* buffer)
{
	INT8U err = OS_NO_ERR;
	char data[] = {COMMAND_GETPACKAGES, '0', ';', COMMAND_EOL};

	// Send command
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_GETPACKAGES, buffer);
	OSSemPost(ReceiveDataSem);
}

void GetMessages(int truckid, char* buffer)
{
	INT8U err = OS_NO_ERR;
	char data[] = {COMMAND_GETMSGS, '0', ';', COMMAND_EOL};

	// Send command
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_GETMSGS, buffer);
	OSSemPost(ReceiveDataSem);
}

void SendMessage(int truckid, char* msg)
{
	INT8U err = OS_NO_ERR;
	int i;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_SENDMSG;
	/*data[1] = (char)truckid;*/
	data[1] = '0';

	for(i=0; msg[i] != COMMAND_EOL; i++) {
		data[i+2] = msg[i];
	}

	data[i+2] = ';';
	data[i+3] = COMMAND_EOL;

	// Send message
	OSSemPend(ReceiveDataSem, 0, &err);
	TransmitRfBuffer(data);
	OSSemPost(ReceiveDataSem);

}

void CodeBarreInit()
{
	BCR_Enable();
}

void CodeBarreRead(char* code)
{
	INT8U err;

	OSFlagPend(bcFlag, 
		BAR_CODE_AVAILABLE,
		OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
		0,
		&err);
	strcpy(code,BCRValue); 
	OSFlagPost(bcFlag, 
		BAR_CODE_CONSUMED, 
		OS_FLAG_SET, 
		&err);
}

void CodeBarreDisable()
{
	BCR_Disable();
}
