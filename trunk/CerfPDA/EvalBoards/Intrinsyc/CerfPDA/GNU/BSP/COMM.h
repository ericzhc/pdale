#ifndef COMM_H
#define COMM_H

#define STATE_UNPICKED 0
#define STATE_PICKED 1
#define STATE_UNDELIVERED 2
#define STATE_DELIVERED 3

void GetNomsCamions(char*); // Fills parameter pointer
short IsValidColis(char*);
void GetPacketInfos(char*); // Define sequence order (seperate each field with ;)
void SetPacketState(short); // Use STATES defined above as parameter
void GetAllPackages(short, char*); // Retrieve all packages and their status associated with a truck

void CodeBarreInit();
void CodeBarreRead(char*);  // Reads the barcode and fills the char*
void CodeBarreDisable();    // Enables the GPS and disables the barcode reader

char* BUFFER_IMAGE;

void GetMessages(char*);
void SendMessage(char*);

#endif