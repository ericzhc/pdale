/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Local variables
*******************************************************/

// Required
OS_STK BCRUpdateTaskStk[TASK_BCR_SIZE];
INT8U enable = 0;

void BCR_Init(void) 
{
	INT8U err;
	#if DEBUG
		printf("Starting BCR update task\n\r");
	#endif
	bcFlag = OSFlagCreate(0x00, &err);
	ComDriverInit(BCREADER_CONFIG);
	OSTaskCreateExt(BCRUpdateTask,
                NULL,
                (OS_STK *)&BCRUpdateTaskStk[TASK_BCR_SIZE-1],
                TASK_BCR_PRIO,
                TASK_BCR_PRIO,
                (OS_STK *)&BCRUpdateTaskStk[0],
                TASK_BCR_SIZE,
                NULL,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
	);
	#if DEBUG
		printf("Init done...BCR\n\r");
	#endif
}

void BCRUpdateTask() 
{
	INT8U err;
	#if DEBUG
		printf("In the BCRUpdateTask()\n\r");
	#endif
	COM_BUFF_INFO RxBuff = GetTaskRxComBuff();
	int i;
	INT16U timeout;

	while(1)
	{
		timeout = 0;
		i = 0;
		memset(BCRValue, 0x00, MAX_BARCODE_LENGTH);
		while (err != OS_TIMEOUT) {
			
			OSFlagPend(comFlag, 
				RX_SERIAL_DATA_AVAILABLE, 
				OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
				timeout,
				&err);
			
			if(err != OS_TIMEOUT)
			{
				#if DEBUG
					printf("Received flagCurrent : %d  End: %d\n\r", *RxBuff.ptrCurrent, *RxBuff.ptrEnd);
				#endif
				timeout = 1000;

				while ((*RxBuff.ptrCurrent != *RxBuff.ptrEnd) && (i<MAX_BARCODE_LENGTH)) {
					printf("%d", i);
					*(RxBuff.ptrCurrent) = (*(RxBuff.ptrCurrent)+1) % (int)SERIAL_BUFF_SIZE;
					BCRValue[i] = RxBuff.Buffer[*(RxBuff.ptrCurrent)];
					i++;
				}
				
				BCRValue[i] = '\0';
				#if DEBUG
					printf("\n\rWhile OVER\n\r");
				#endif
			}	
		}
		OSFlagPost(bcFlag, 
			BAR_CODE_AVAILABLE,
			OS_FLAG_SET, 
			&err);
		OSFlagPend(bcFlag, 
			BAR_CODE_CONSUMED, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);

		#if DEBUG
			printf("BCR Update Task\n\r");
		#endif
	}
}

void BCR_Enable()
{
	if(enable == 0)
	{
		BCR_Init();
		GPS_Disable();
		enable = 1;
	}
	else
	{
		GPS_Disable();
		SetCTS();
		OSTaskResume(TASK_BCR_PRIO);
	}
}

void BCR_Disable()
{
	GPS_Enable();
	ClearCTS();
	OSTaskSuspend(TASK_BCR_PRIO);
}
