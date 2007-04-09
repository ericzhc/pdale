#ifndef GPS_H
#define GPS_H

#define TSIP_BUFF_SIZE 8
#define TASK_GPS_PRIO 8
#define TASK_GPS_SIZE 1024

#define GPS_DLE 0x10
#define GPS_ID 0x2B // See documentation Lassen Page 106
#define GPS_TIME 0x41
#define	GPS_ETX 0x03

#define SECONDS_IN_ONE_DAY 60*60*24

typedef struct {
	char Latitude[6];
	char Longitude[6];
	char Altitude[6];
} GPSCoord;

typedef struct {
	// See documentation Lassen Page 115
	int Hours;
	int Minutes;
	int Seconds;
} GPSTime;

void GPS_Init();
void GPSUpdateTask();
void ReadPosition();
void ReadTime();
void GPS_Disable();
void GPS_Enable();
void ReceivedTSIP();

#endif
