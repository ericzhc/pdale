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
// GPS position that is updated locally and read from external apps
GPSCoord GPSPosition;
// GPS Time
GPSTime GPSTimeValue;
// Required
OS_STK GPSUpdateTaskStk[TASK_GPS_SIZE];
INT8U err;

void GPS_Init(void) 
{
	ComDriverInit(GPS_CONFIG);

	strcpy(GPSPosition.Latitude, "1.000");
	strcpy(GPSPosition.Longitude, "2.000");
	strcpy(GPSPosition.Altitude, "3.000");

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
		while (RxBuff.ptrCurrent != RxBuff.ptrEnd) {
			*(RxBuff.ptrCurrent) = (*(RxBuff.ptrCurrent)+1) % (int)SERIAL_BUFF_SIZE;
			ptrGPSBuffEnd = (ptrGPSBuffEnd+1) % (int)TSIP_BUFF_SIZE;
			
			GPSBuffer[ptrGPSBuffEnd] = RxBuff.Buffer[*(RxBuff.ptrCurrent)];
		}
		ReceivedTSIP();
		OSTimeDlyHMSM(0,0,0,10);
		printf("GPS Update Task\n\r");
	}
}

void ReceivedTSIP()
{
	while (ptrGPSBuffCurr != ptrGPSBuffEnd) {
		printf("TSIP Detected\n\r");
		ptrGPSBuffCurr = (ptrGPSBuffCurr+1) % (int)TSIP_BUFF_SIZE;
		switch (GPSBuffer[ptrGPSBuffCurr]) {
			case GPS_DLE : // synchronisation DLE
				break;
			case GPS_ID : // position id
				ReadPosition();
				break;
			case GPS_TIME : // read time
				ReadTime();
				break;
			case GPS_ETX : // synchronisation ETX
				break;
			default : // unsupported packet, skip it (until DLE is met)
				while ((GPSBuffer[ptrGPSBuffCurr] != 0x10) || (ptrGPSBuffCurr != ptrGPSBuffEnd)) {
					ptrGPSBuffCurr++;
				}
				break;
		}
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
	GPSPosition.Latitude[0] = GPSBuffer[ptrGPSBuffCurr];
	GPSPosition.Latitude[2] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Latitude[3] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Latitude[4] = GPSBuffer[++ptrGPSBuffCurr];

	GPSPosition.Longitude[0] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Longitude[2] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Longitude[3] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Longitude[4] = GPSBuffer[++ptrGPSBuffCurr];

	GPSPosition.Altitude[0] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Altitude[2] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Altitude[3] = GPSBuffer[++ptrGPSBuffCurr];
	GPSPosition.Altitude[4] = GPSBuffer[++ptrGPSBuffCurr];
}

void ReadTime() 
{
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
