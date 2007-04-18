/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Local variables
*******************************************************/
// Required
OS_STK MapUpdateTaskStk[TASK_MAP_SIZE];

void MAP_Init(void) 
{
	erD_sndstr("Starting MAP update task\n\r");
	erD_sndstr("Init done...Map\n\r");
}

void MapUpdateTask() 
{
	while (1) {
		char data[] = {COMMAND_GETMAP, COMMAND_EOL};

		OSTimeDlyHMSM(0,MAPDELAY,0,0);

		// Send command
		TransmitRfBuffer(data);
		// Wait for response
		memset(CurrentMap, 0x0, MAX_MAP_SIZE);
		ReceiveData(COMMAND_GETMAP, CurrentMap);
	}
}
//
//void ReceiveMap(char cmd, char* buffer)
//{
//	INT8U err;
//	// Wait for the data to be received
//	OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0,&err);
//
//	int i = 0;
//	while((RxRfSerialBuffer[ptrRfRxBuffCurr] != COMMAND_EOL)) {
//		ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
//		buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
//		if ((ptrRfRxBuffCurr == ptrRfRxBuffEnd)) {
//			OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, OS_TICKS_PER_SEC,&err); // wait one second for a next input
//		}
//		i++;
//	}
//}
