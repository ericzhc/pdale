/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Local variables
*******************************************************/
// Barcode value read
char BCRValue[MAX_BARCODE_LENGTH];
// Required
OS_STK BCRUpdateTaskStk[TASK_BCR_SIZE];

void BCR_Init(void) 
{
	#if DEBUG
		printf("Starting BCR update task\n\r");
	#endif
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
	COM_BUFF_INFO RxBuff = GetTaskRxComBuff();
	int i;
	while (1) {
		i = 0;
		while ((RxBuff.ptrCurrent != RxBuff.ptrEnd) && (i<MAX_BARCODE_LENGTH)) {
			*(RxBuff.ptrCurrent) = (*(RxBuff.ptrCurrent)+1) % (int)SERIAL_BUFF_SIZE;
			BCRValue[i] = RxBuff.Buffer[*(RxBuff.ptrCurrent)];
		}
		OSTimeDlyHMSM(0,0,0,10);
		#if DEBUG
			printf("BCR Update Task\n\r");
		#endif
	}
}

void BCR_Enable()
{
	SetCTS();
	OSTaskResume(TASK_BCR_PRIO);
}

void BCR_Disable()
{
	ClearCTS();
	OSTaskSuspend(TASK_BCR_PRIO);
}
