/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Local variables
*******************************************************/
// Required


void MAP_Init(void) 
{
	erD_sndstr("Starting MAP update task\n\r");
	erD_sndstr("Init done...Map\n\r");
}

void MapUpdateTask() 
{
	INT8U err = OS_NO_ERR;
	erD_sndstr("Starting MAP update task\n\r");
	while (1) {
		OSTimeDlyHMSM(0,MAPDELAY,0,0);
		char data[] = {COMMAND_GETMAP, ';',COMMAND_EOL};

		

		memset(CurrentMap, 0x0, MAX_MAP_SIZE);
		OSSemPend(ReceiveDataSem, 0, &err);
		erD_sndstr("Requesting map");
		erD_snd_cr();
		// Send command
		TransmitRfBuffer(data);
		// Wait for response

		ReceiveData(COMMAND_GETMAP, CurrentMap);
		OSSemPost(ReceiveDataSem);
		erD_sndstr("Map Received");
		erD_snd_cr();
	}
}
int getMap(char* buffer) {
	int i =0;
	char mapLength[10];
	while (CurrentMap[i] != ';') {
		mapLength[i] = CurrentMap[i];
		i++;
	}
	i++;
	buffer = &(CurrentMap[i]);
	mapLength[i] = '\0';
	int length = atoi(mapLength);
	//printf("Map lenght %d", length);
	return length;
}

/*void ReceiveMap(char* buffer)
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
*/
