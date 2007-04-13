/******************************************************
    Includes
*******************************************************/
#include <includes.h>

/******************************************************
    Local variables
*******************************************************/
// Temp buffer to receive TSIP packets
char GPSBuffer[TSIP_BUFF_SIZE];
// Buffer pointers
int ptrGPSBuffCurr = 0, ptrGPSBuffEnd = 0;
// Required
OS_STK GPSUpdateTaskStk[TASK_GPS_SIZE];
INT8U err;

void GPS_Init(void) 
{
	ComDriverInit(GPS_CONFIG);

	/*memset(GPSPosition.Latitude, 0x00, 10);
	memset(GPSPosition.Longitude, 0x00, 10);
	memset(GPSPosition.Altitude, 0x00, 10);*/

	//strcpy(GPSPosition.Latitude, "1.000");
	//strcpy(GPSPosition.Longitude, "2.000");
	//strcpy(GPSPosition.Altitude, "3.000");

	#if DEBUG
		printf("Starting GPS update task\n\r");
	#endif
	OSTaskCreateExt(GPSUpdateTask,
                NULL,
                (OS_STK *)&GPSUpdateTaskStk[TASK_GPS_SIZE-1],
                TASK_GPS_PRIO,
                TASK_GPS_PRIO,
                (OS_STK *)&GPSUpdateTaskStk[0],
                TASK_GPS_SIZE,
                NULL,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
	);
	#if DEBUG
		printf("Init done...GPS\n\r");
	#endif
}

void GPSUpdateTask() 
{
	COM_BUFF_INFO RxBuff = GetTaskRxComBuff();
	while (1) {
		OSFlagPend(comFlag, 
			RX_SERIAL_DATA_AVAILABLE, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);
		while (*(RxBuff.ptrCurrent) != *(RxBuff.ptrEnd)) {
			*(RxBuff.ptrCurrent) = (*(RxBuff.ptrCurrent)+1) % (int)SERIAL_BUFF_SIZE;
			//printf("Current: %d - End: %d\n\r", *(RxBuff.ptrCurrent), *(RxBuff.ptrEnd));
			//printf("--- Chars : %x \n\r", RxBuff.Buffer[*(RxBuff.ptrCurrent)]);
			ptrGPSBuffEnd = (ptrGPSBuffEnd+1) % (int)TSIP_BUFF_SIZE;
			GPSBuffer[ptrGPSBuffEnd] = RxBuff.Buffer[*(RxBuff.ptrCurrent)];
		}
		ReceivedTSIP();
		//OSTimeDlyHMSM(0,0,0,10);
		//printf("GPS Update Task\n\r");
	}
}

void ReceivedTSIP()
{
	//int condition = (ptrGPSBuffCurr+13) % (int)TSIP_BUFF_SIZE;
	
	int condition = 0;
	if (ptrGPSBuffCurr < ptrGPSBuffEnd) {
		condition = ptrGPSBuffEnd - ptrGPSBuffCurr;
	} else if (ptrGPSBuffCurr > ptrGPSBuffEnd) {
		condition = ((int)TSIP_BUFF_SIZE - ptrGPSBuffCurr) + ptrGPSBuffEnd;
	}
	printf("Current: %d - End: %d - Condition: %d\n\r", ptrGPSBuffCurr, ptrGPSBuffEnd, condition);
	while ((ptrGPSBuffCurr != ptrGPSBuffEnd) && (condition > 14)) {
		
		ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
		
		if((GPSBuffer[ptrGPSBuffCurr] == (char) GPS_DLE)) {
			
			ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
			
			if ((GPSBuffer[ptrGPSBuffCurr] != (char) GPS_ETX)) {
				
				if((GPSBuffer[ptrGPSBuffCurr] == (char) GPS_POS)) {
					ReadPosition();
				} else if(GPSBuffer[ptrGPSBuffCurr] == (char) GPS_TIME) {
					ReadTime();
				}
			}
		}
		if (ptrGPSBuffCurr < ptrGPSBuffEnd) {
			condition = ptrGPSBuffEnd - ptrGPSBuffCurr;
		} else if (ptrGPSBuffCurr > ptrGPSBuffEnd) {
			condition = ((int)TSIP_BUFF_SIZE - ptrGPSBuffCurr) + ptrGPSBuffEnd;
		}
		printf("Current: %d - End: %d - Condition: %d\n\r", ptrGPSBuffCurr, ptrGPSBuffEnd, condition);
	}
}

// Sends the TSIP packets for GPS configuration
void SendTSIP(char *data) 
{
	TransmitBuffer(data);
}

void ReadPosition() 
{
/* Each value is of type: Single — Float, or 4 byte REAL has a precision of 
					24 significant bits, roughly 6.5 digits. Page 87 Lassen Doc
*/
	// Documentation Page 109 for TSIP structure of a LLA packet
	printf("Reading position");

	char GPSData[4];

	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[3] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[2] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[1] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[0] = GPSBuffer[ptrGPSBuffCurr];

	float* mySecondFloat = (float*)GPSData;
	GPSPosition.Latitude = (((float)*mySecondFloat) * 180) /  3.14159265;

	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[3] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[2] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[1] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[0] = GPSBuffer[ptrGPSBuffCurr];

	mySecondFloat = (float*)GPSData;
	GPSPosition.Longitude = (((float)*mySecondFloat) * 180) /  3.14159265;

	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[3] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[2] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[1] = GPSBuffer[ptrGPSBuffCurr];
	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	GPSData[0] = GPSBuffer[ptrGPSBuffCurr];

	mySecondFloat = (float*)GPSData;
	GPSPosition.Altitude = ((float)*mySecondFloat);
}

void ReadTime() 
{
	printf("Reading time...\n\r");

	ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
	int seconds = atoi(&(GPSBuffer[ptrGPSBuffCurr]));
	if (seconds < SECONDS_IN_ONE_DAY) {
	} else if (seconds < SECONDS_IN_ONE_DAY*2) { // Monday
		seconds -= SECONDS_IN_ONE_DAY;
	} else if (seconds < SECONDS_IN_ONE_DAY*3) { // Tuesday
		seconds -= SECONDS_IN_ONE_DAY * 2;
	} else if (seconds < SECONDS_IN_ONE_DAY*4) { // Wednesday
		seconds -= SECONDS_IN_ONE_DAY * 3;
	} else if (seconds < SECONDS_IN_ONE_DAY*5) { // Thursday
		seconds -= SECONDS_IN_ONE_DAY * 4;
	} else if (seconds < SECONDS_IN_ONE_DAY*6) { // Friday
		seconds -= SECONDS_IN_ONE_DAY * 5;
	} else if (seconds < SECONDS_IN_ONE_DAY*7) { // Saturday
		seconds -= SECONDS_IN_ONE_DAY * 6;
	}

	GPSTimeValue.Hours   = seconds / (60*60);
	GPSTimeValue.Minutes = (seconds / (24*60)) - (GPSTimeValue.Hours*60);
	GPSTimeValue.Seconds = (seconds / (24*60*60)) - (GPSTimeValue.Hours*60*60) - (GPSTimeValue.Minutes*60);
}

void GPS_Enable()
{
	// Send TSIP packet to start automatic transmission of GPS data
	char trame[] = {0x35, 0x02, 0x01}; // page 89
	SendTSIP(trame);
	OSTaskResume(TASK_GPS_PRIO);
}

void GPS_Disable()
{
	// Send TSIP packet to stop automatic transmission of GPS data
	char trame[] = {0x35, 0x00, 0x00}; // page 89
	SendTSIP(trame);
	OSTaskSuspend(TASK_GPS_PRIO);
}
