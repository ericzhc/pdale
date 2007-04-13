#ifndef GPS_H
#define GPS_H

#define TSIP_BUFF_SIZE 1024
#define TASK_GPS_PRIO 8
#define TASK_GPS_SIZE 1024

#define GPS_DLE 0x10
#define GPS_POS 0x4A // See documentation Lassen Page 106
#define GPS_TIME 0x41
#define	GPS_ETX 0x03

#define SECONDS_IN_ONE_DAY 60*60*24

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

// GPS position that is updated locally and read from external apps
GPSCoord GPSPosition;
// GPS Time
GPSTime GPSTimeValue;

void GPS_Init();
void GPSUpdateTask();
void ReadPosition();
void ReadTime();
void GPS_Disable();
void GPS_Enable();
void ReceivedTSIP();

#endif
