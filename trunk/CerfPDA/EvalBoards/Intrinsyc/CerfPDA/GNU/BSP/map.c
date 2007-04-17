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
	#if DEBUG
		printf("Starting MAP update task\n\r");
	#endif
	OSTaskCreateExt(MapUpdateTask,
                NULL,
                (OS_STK *)&MapUpdateTaskStk[TASK_MAP_SIZE-1],
                TASK_MAP_PRIO,
                TASK_MAP_PRIO,
                (OS_STK *)&MapUpdateTaskStk[0],
                TASK_MAP_SIZE,
                NULL,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
	);
	#if DEBUG
		printf("Init done...Map\n\r");
	#endif
}

void MapUpdateTask() 
{
	while (1) {
			INT8U err;
			char data[] = {COMMAND_GETMAP, COMMAND_EOL};

			OSTimeDlyHMSM(0,MAPDELAY,0,0);

			// Send command
			TransmitRfBuffer(data);
			// Wait for response
			ReceiveMap(COMMAND_GETMAP, CurrentMap);
	}
}

void ReceiveMap(char cmd, char* buffer)
{
	INT8U err;
	// Wait for the data to be received
	OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0,&err);

	int i = 0;
	while((RxRfSerialBuffer[ptrRfRxBuffCurr] != COMMAND_EOL)) {
		ptrRfRxBuffCurr = (ptrRfRxBuffCurr + 1) % (int) SERIAL_BUFF_SIZE;
		buffer[i] = RxRfSerialBuffer[ptrRfRxBuffCurr];
		if ((ptrRfRxBuffCurr == ptrRfRxBuffEnd)) {
			OSFlagPend(RfFlag, TCP_TRANSFER_RECEIVED, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, OS_TICKS_PER_SEC,&err); // wait one second for a next input
		}
		i++;
	}
}
