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
	printf("Wait for the data to be received");
	//while (err != OS_TIMEOUT) {
	while (1) {
		OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, timeout,&err);
		timeout = 5000;

		if(err == OS_NO_ERR)
		{
			while(ptrRfRxBuffCurr != ptrRfRxBuffEnd) {
				ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
				buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
				printf("char recu: %c\n\r", RxRfSerialBuffer[ptrRfRxBuffCurr]);
				i++;
			}
		}
	}
	buffer[i] = '\0';
	printf("Messages recus : %s\n\r", buffer);
}

void GetTruckNames(char* buffer)
{
	INT8U err;
	char data[] = {COMMAND_TRUCKNAMES, COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_TRUCKNAMES, buffer);
}

char IsValidPackage(int packetid)
{
	INT8U err;
	char data[] = {COMMAND_VALIDPACKAGE, (char)packetid, COMMAND_EOL};
	char tempbuff[2];

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_VALIDPACKAGE, tempbuff);

	return tempbuff[0];
}

void GetPacketInfos(int packetid, char* buffer)
{
	INT8U err;
	char data[] = {COMMAND_PACKETINFOS, (char)packetid, COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_PACKETINFOS, buffer);
}

void SetPacketState(int packetid, int value)
{
	INT8U err;
	char data[4] = {COMMAND_SETPACKETSTATE, (char)packetid, (char)value, COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
}

void GetAllPackages(int truckid, char* buffer)
{
	INT8U err;
	char data[] = {COMMAND_GETPACKAGES, truckid, COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_GETPACKAGES, buffer);
}

void GetMessages(int truckid, char* buffer)
{
	INT8U err;
	char data[] = {COMMAND_GETMSGS, '0', ';', COMMAND_EOL};

	// Send command
	TransmitRfBuffer(data);
	// Wait for response
	ReceiveData(COMMAND_GETMSGS, buffer);
}

void SendMessage(int truckid, char* msg)
{
	INT8U err;
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
