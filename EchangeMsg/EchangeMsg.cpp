#include <winsock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")

using namespace::std;

int main()
{

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,0),&wsa); 

    SOCKET sock;
    SOCKADDR_IN sockin;

    cout << "Programme test pour l'echange de messages avec l'interface PC" << endl;

    int portEcoute = 12000;

    cout << "Ecoute sur le port " << portEcoute << endl;

    sockin.sin_family = AF_INET;
    sockin.sin_addr.s_addr = INADDR_ANY;
    sockin.sin_port = htons(portEcoute);

    sock = socket(AF_INET, SOCK_STREAM, 0); 
    bind(sock, (SOCKADDR*)&sockin, sizeof(sockin));

    listen(sock, 0);
    SOCKET connSock = 0;
    
    char buffer[50];//Pour stocker les packets à envoyer
    int sockinSize = sizeof(sockin);

    //On attend l'arrivee d'un message et on envoit une confirmation de la reception
    while(1)
    {
        connSock = accept(sock, (SOCKADDR *)&sockin, &sockinSize);
        if(connSock != INVALID_SOCKET)
        {
            recv(connSock, buffer, sizeof(buffer), 0);
            
            cout << "Message : " << buffer << endl;

            SOCKET sendSock;
            SOCKADDR_IN sockout;

            sockout.sin_family = AF_INET;
            sockout.sin_addr.s_addr = inet_addr("127.0.0.1");
            sockout.sin_port = htons(12001);

            sendSock = socket(AF_INET, SOCK_STREAM, 0);

            connect(sendSock, (SOCKADDR *)&sockout, sizeof(sockout));
            send(sendSock, "Message reçu\r\n", 14, 0);        
        }
    }

    return 1;
}