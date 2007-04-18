#include <includes.h>

void Tasks() 
{
	INT8U err;
	InitFlag = OSFlagCreate(0x0, &err);
	OSTaskCreateExt(BufferRfTransmissionTask,
					NULL,
					(OS_STK *)&BufferRfTransmissionTaskStk[TASK_RFSERIAL_SIZE-1],
					TASK_RFSERIAL_PRIO,
					TASK_RFSERIAL_PRIO,
					(OS_STK *)&BufferRfTransmissionTaskStk[0],
					TASK_RFSERIAL_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskCreateExt(GPSUpdateTask,
					NULL,
					(OS_STK *)&GPSUpdateTaskStk[TASK_GPS_SIZE-1],
					TASK_GPS_PRIO,
					TASK_GPS_PRIO,
					(OS_STK *)&GPSUpdateTaskStk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	OSTaskCreateExt(GPSSendDataTask,
					NULL,
					(OS_STK *)&GPSSendDataTaskTsk[TASK_GPS_SIZE-1],
					TASK_GPS_SEND_PRIO,
					TASK_GPS_SEND_PRIO,
					(OS_STK *)&GPSSendDataTaskTsk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	//OSTaskCreateExt(MapUpdateTask,
	//				NULL,
	//				(OS_STK *)&MapUpdateTaskStk[TASK_MAP_SIZE-1],
	//				TASK_MAP_PRIO,
	//				TASK_MAP_PRIO,
	//				(OS_STK *)&MapUpdateTaskStk[0],
	//				TASK_MAP_SIZE,
	//				NULL,
	//				OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
	//				);
}
