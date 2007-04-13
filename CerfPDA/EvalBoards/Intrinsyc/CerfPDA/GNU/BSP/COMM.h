#ifndef COMM_H
#define COMM_H

#define STATE_UNPICKED 0
#define STATE_PICKED 1
#define STATE_UNDELIVERED 2
#define STATE_DELIVERED 3

#define MAX_MSG_SIZE 100

#define COMMAND_EOL '\0'
#define COMMAND_TRUCKNAMES 0x30
#define COMMAND_VALIDPACKAGE 0x31
#define COMMAND_PACKETINFOS 0x32
#define COMMAND_SETPACKETSTATE 0x33
#define COMMAND_GETPACKAGES 0x34
#define COMMAND_GETMSGS 0x35
#define COMMAND_SENDMSG 0x36

void ReceiveData(char, char*);
void GetTruckNames(char*); // Fills parameter pointer
char IsValidPackage(int);  // returns 0 or 1 to specify is package is valid
void GetPacketInfos(int, char*); // Define sequence order (seperate each field with ;)
void SetPacketState(int, int); // Use STATES defined above as parameter
void GetAllPackages(int, char*); // Retrieve all packages and their status associated with a truck
void GetMessages(int, char*);
void SendMessage(int, char*);

char* BUFFER_IMAGE;

void CodeBarreInit();
void CodeBarreRead(char*);  // Reads the barcode and fills the char*
void CodeBarreDisable();    // Enables the GPS and disables the barcode reader

#endif
