#ifndef GPS_H
#define GPS_H

/*
*********************************************************************************************************
*                                        DEFINITION DES STRUCTURES
*********************************************************************************************************
*/

typedef struct {
	float Latitude;
	float Longitude;
	float Altitude;
} GPSCoord;

typedef struct {
	// See documentation Lassen Page 115
	int Hours;
	int Minutes;
	int Seconds;
} GPSTime;
/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/
#define TSIP_BUFF_SIZE 1024
#define TASK_GPS_SIZE 1024

OS_STK GPSUpdateTaskStk[TASK_GPS_SIZE];
OS_STK GPSSendDataTaskTsk[TASK_GPS_SIZE];

#define GPS_DLE 0x10
#define GPS_POS 0x4A // See documentation Lassen Page 106
#define GPS_TIME 0x41
#define	GPS_ETX 0x03

#define GPSDELAY 101 // Time to update the GPS position on the remote server in minutes
#define SECONDS_IN_ONE_DAY 60*60*24
// GPS position that is updated locally and read from external apps
GPSCoord GPSPosition;
// GPS Time
GPSTime GPSTimeValue;

OS_FLAG_GRP* gpsFlag;
#define GPS_READY_TO_SEND		1
/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void GPS_Init();
void GPSUpdateTask();
void GPSSendDataTask();
void ReadPosition();
void ReadTime();
void GPS_Disable();
void GPS_Enable();
void ReceivedTSIP();

#endif
