/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Methods
*******************************************************/

void ReceiveData(char cmd, char* buffer)
{
	INT8U err;
	// Wait for the data to be received
	OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0,&err);

	int i = 0;
	while(ptrRfRxBuffCurr != ptrRfRxBuffEnd) {
		ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
		buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
		i++;
	}
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
	char data[] = {COMMAND_GETMSGS, (char)truckid, COMMAND_EOL};

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
	data[1] = (char)truckid;

	for(i=0; msg[i] != COMMAND_EOL; i++) {
		data[i+2] = msg[i];
	}

	data[i+2] = COMMAND_EOL;

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
