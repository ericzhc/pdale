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
int firstGPSstart = 1;
/*******************************************************
 Initialize GPS interface
*******************************************************/
void GPS_Init(void) 
{
	INT8U err;

	if (firstGPSstart) {
		ComDriverInit(GPS_CONFIG);
	}

	erD_sndstr("Starting GPS update task\n\r");

	gpsFlag = OSFlagCreate(0x00, &err);
	erD_sndstr("Init done...GPS\n\r");

	OSFlagPost(InitFlag, GPS_INIT_DONE, OS_FLAG_SET, &err);
	OSFlagPost(InitFlag, GPS_INIT_DONE2, OS_FLAG_SET, &err);
}

/*******************************************************
 Sends last read GPS coordonates to remote server
*******************************************************/
void GPSSendDataTask() 
{
	INT8U err;
	
	OSFlagPend(InitFlag, GPS_INIT_DONE, OS_FLAG_WAIT_SET_ALL, 0, &err);
	erD_sndstr("GPSSendDataTask started...\n\r");
	while (1) {

		OSTimeDlyHMSM(0,1,41,0);

		char data[24];
		memset(data, 'q', 24);
		data[0] = COMMAND_GPSCOORD;
		data[1] = '0';
		sprintf(&data[2], "%2.6f", GPSPosition.Longitude);
		data[12] = ';';
		sprintf(&data[13], "%2.6f", GPSPosition.Latitude);
		data[22] = ';';
		data[23] = COMMAND_EOL;

		erD_sndstr("Data Sent as GPS coordonees : ");
		erD_sndstr(data);
		erD_sndstr("\n\r");
		TransmitRfBuffer(data);
	}
}
/*******************************************************
 Read GPS coordonate at a given rate
*******************************************************/
void GPSUpdateTask() 
{
	INT8U err;
	OSFlagPend(InitFlag, GPS_INIT_DONE2, OS_FLAG_WAIT_SET_ALL, 0, &err);
	COM_BUFF_INFO RxBuff = GetTaskRxComBuff();
	GPSPosition.Longitude = -71.924255;

	GPSPosition.Latitude = 45.377930;
	while (1) {
		OSFlagPend(comFlag, 
			RX_SERIAL_DATA_AVAILABLE, 
			OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 
			0,
			&err);
		while (*(RxBuff.ptrCurrent) != *(RxBuff.ptrEnd)) {
			*(RxBuff.ptrCurrent) = (*(RxBuff.ptrCurrent)+1) % (int)SERIAL_BUFF_SIZE;
			ptrGPSBuffEnd = (ptrGPSBuffEnd+1) % (int)TSIP_BUFF_SIZE;
			GPSBuffer[ptrGPSBuffEnd] = RxBuff.Buffer[*(RxBuff.ptrCurrent)];
		}
		ReceivedTSIP();
		OSFlagPost(gpsFlag, 
			GPS_READY_TO_SEND, 
			OS_FLAG_SET, 
			&err);
	}
}
/*******************************************************
 Get TSIP received for GPS
*******************************************************/
void ReceivedTSIP()
{
	//int condition = (ptrGPSBuffCurr+13) % (int)TSIP_BUFF_SIZE;
	
	int condition = 0;
	if (ptrGPSBuffCurr < ptrGPSBuffEnd) {
		condition = ptrGPSBuffEnd - ptrGPSBuffCurr;
	} else if (ptrGPSBuffCurr > ptrGPSBuffEnd) {
		condition = ((int)TSIP_BUFF_SIZE - ptrGPSBuffCurr) + ptrGPSBuffEnd;
	}
	//printf("Current: %d - End: %d - Condition: %d\n\r", ptrGPSBuffCurr, ptrGPSBuffEnd, condition);
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
		//printf("Current: %d - End: %d - Condition: %d\n\r", ptrGPSBuffCurr, ptrGPSBuffEnd, condition);
	}
}
/*******************************************************
 Sends the TSIP packets for GPS configuration
*******************************************************/ 
void SendTSIP(char *data) 
{
	TransmitBuffer(data);
}
/*******************************************************
 Extract GPS coordinates for last received TSIP
*******************************************************/ 
void ReadPosition() 
{
/* Each value is of type: Single — Float, or 4 byte REAL has a precision of 
					24 significant bits, roughly 6.5 digits. Page 87 Lassen Doc
*/
	// Documentation Page 109 for TSIP structure of a LLA packet
	erD_sndstr("Reading position...\n\r");

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
/*******************************************************
 Extract time for last received TSIP
*******************************************************/
void ReadTime() 
{
	erD_sndstr("Reading time...\n\r");

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
/*******************************************************
 Enable and initialize GPS interface
*******************************************************/
void GPS_Enable()
{
	erD_sndstr("Enabling GPS\n\r");
	// Send TSIP packet to start automatic transmission of GPS data
	//char trame[] = {0x10, 0x35, 0x02, 0x01, 0x10, 0x03, COMMAND_EOL}; // page 89
	//SendTSIP(trame);
	OSTaskCreateExt(GPSUpdateTask,
					NULL,
					(OS_STK *)&GPSUpdateTaskStk[TASK_GPS_SIZE-1],
					TASK_GPS_PRIO,
					TASK_GPS_PRIO,
					(OS_STK *)&GPSUpdateTaskStk[0],
					TASK_GPS_SIZE,
					NULL,
					OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}
/*******************************************************
 Disable GPS interface
*******************************************************/
void GPS_Disable()
{
	erD_sndstr("Disabling GPS\n\r");
	// Send TSIP packet to stop automatic transmission of GPS data
	char trame[] = {0x10, 0x35, 0x00, 0x00, 0x10, 0x03, COMMAND_EOL}; // page 89
	SendTSIP(trame);
	OSTaskDel(TASK_GPS_PRIO);
}
