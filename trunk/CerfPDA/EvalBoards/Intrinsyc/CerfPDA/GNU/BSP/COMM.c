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
	// Wait for the data to be received
	int i = 0;
	int timeout = 0;
	//erD_sndstr("Wait for the data to be received");
	while (err != OS_TIMEOUT) {
		OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, timeout,&err);
		timeout = 5000;

		if(err == OS_NO_ERR)
		{
			while(ptrRfRxBuffCurr != ptrRfRxBuffEnd) {
				ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
				buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
				//erD_sndstr("char recu: %c\n\r", RxRfSerialBuffer[ptrRfRxBuffCurr]);
				i++;
			}
		}
	}
	buffer[i] = '\0';
	erD_sndstr("Messages recus : ");
	erD_sndstr(buffer);
}

void GetTruckNames(char* buffer)
{
	char data[] = {COMMAND_TRUCKNAMES, ';', COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_TRUCKNAMES, buffer);
}

char IsValidPackage(char* packetid)
{
	int i;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_VALIDPACKAGE;

	for(i=0; packetid[i] != COMMAND_EOL; i++) {
		data[i+1] = packetid[i];
	}

	data[i+1] = ';';
	data[i+2] = COMMAND_EOL;

	// Send message
	TransmitRfBuffer(data);

	//char data[] = {COMMAND_VALIDPACKAGE, packetid, ';', COMMAND_EOL};
	char tempbuff[2];

	// Wait for response
	ReceiveData(COMMAND_VALIDPACKAGE, tempbuff);

	return tempbuff[0];
}

void GetPacketInfos(char* packetid, char* buffer)
{
	int i;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_PACKETINFOS;

	for(i=0; packetid[i] != COMMAND_EOL; i++) {
		data[i+1] = packetid[i];
	}

	data[i+1] = ';';
	data[i+2] = COMMAND_EOL;

	// Send message
	TransmitRfBuffer(data);

	// Wait for response
	ReceiveData(COMMAND_PACKETINFOS, buffer);
}

void SetPacketState(char* packetid, char value)
{
	int i;
	char data[MAX_MSG_SIZE];
	data[0] = COMMAND_SETPACKETSTATE;
	data[1] = value;
	for(i=0; packetid[i] != COMMAND_EOL; i++) {
		data[i+2] = packetid[i];
	}

	data[i+2] = ';';
	data[i+3] = COMMAND_EOL;

	// Send command
	TransmitRfBuffer(data);
}

void GetAllPackages(int truckid, char* buffer)
{
	char data[] = {COMMAND_GETPACKAGES, '1', ';', COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_GETPACKAGES, buffer);
}

void GetMessages(int truckid, char* buffer)
{
	char data[] = {COMMAND_GETMSGS, '0', ';', COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_GETMSGS, buffer);
}

void SendMessage(int truckid, char* msg)
{
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
	TransmitRfBuffer(data);
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
