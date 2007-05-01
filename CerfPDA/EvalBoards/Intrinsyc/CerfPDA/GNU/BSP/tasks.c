#include <includes.h>
/*******************************************************
 Creates GPS and Map related tasks
*******************************************************/
void Tasks() 
{
	INT8U err;
	InitFlag = OSFlagCreate(0x0, &err);
	// Create GPSUpdateTask
	OSTaskCreateExt(GPSUpdateTask,
					NULL,
					(OS_STK *)&GPSUpdateTaskStk[TASK_GPS_SIZE-1],
					TASK_GPS_PRIO,
					TASK_GPS_PRIO,
					(OS_STK *)&GPSUpdateTaskStk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	// Create GPSSendDataTask
	OSTaskCreateExt(GPSSendDataTask,
					NULL,
					(OS_STK *)&GPSSendDataTaskTsk[TASK_GPS_SIZE-1],
					TASK_GPS_SEND_PRIO,
					TASK_GPS_SEND_PRIO,
					(OS_STK *)&GPSSendDataTaskTsk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	// Create MapUpdateTask
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
}
