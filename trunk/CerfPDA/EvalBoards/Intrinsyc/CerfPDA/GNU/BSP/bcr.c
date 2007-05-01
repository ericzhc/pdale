/******************************************************
    Includes
*******************************************************/
#include <includes.h>
#include <serial_front.h>

/******************************************************
    Local variables
*******************************************************/

// Required
OS_STK BCRUpdateTaskStk[TASK_BCR_SIZE];
static INT8U enable = 0;
/******************************************************
  Enable and initalize bar code reader
*******************************************************/
void BCR_Init(void) 
{
	enable = 1;
	INT8U err;
	
	erD_sndstr("Starting BCR update task\n\r");
	
	bcFlag = OSFlagCreate(0x00, &err);
	//ComDriverInit(BCREADER_CONFIG);
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

	erD_sndstr("Init done...BCR\n\r");
}
/******************************************************
  Read serial for bcr data
*******************************************************/
void BCRUpdateTask() 
{
	INT8U err;

	erD_sndstr("In the BCRUpdateTask()\n\r");
	
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
			if(err == OS_NO_ERR)
			{
				timeout = 3000;

				while ((*RxBuff.ptrCurrent != *RxBuff.ptrEnd) && (i<MAX_BARCODE_LENGTH)) 
				{
					printf("%d", i);
					*(RxBuff.ptrCurrent) = (*(RxBuff.ptrCurrent)+1) % (int)SERIAL_BUFF_SIZE;
					BCRValue[i] = RxBuff.Buffer[*(RxBuff.ptrCurrent)];
					i++;
				}
				
				BCRValue[i] = '\0';
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
/******************************************************
  Enable bar code reader
*******************************************************/
void BCR_Enable()
{
	COM_BUFF_INFO RxBuff = GetTaskRxComBuff();

	if(enable == 0)
	{
		GPS_Disable();
		BCR_Init();
	}
	else
	{
		GPS_Disable();
		//SetCTS();
		OSTaskResume(TASK_BCR_PRIO);
	}

	*(RxBuff.ptrCurrent) = *(RxBuff.ptrEnd);
}
/******************************************************
  Disable bar code reader
*******************************************************/
void BCR_Disable()
{
	COM_BUFF_INFO RxBuff = GetTaskRxComBuff();

	OSTaskSuspend(TASK_BCR_PRIO);
    *(RxBuff.ptrCurrent) = *(RxBuff.ptrEnd);
	GPS_Enable();
}
