/*
*********************************************************************************************************
* Fichier : Program.cs
* Par     : Marc-Étienne Lebeau, Julien Beaumier-Ethier, Richard Labonté
* Date    : 2007/04/12
*********************************************************************************************************
*/
using Socket.net.mappoint.staging;
using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using Socket.Data;

namespace Socket.TCPServerReceiver
{
    class Program
    { 
        //Constantes qui remplacent les #define de comm.h
        const int STATE_UNPICKED = 0;
        const int STATE_PICKED = 1;
        const int STATE_UNDELIVERED = 2;
        const int STATE_DELIVERED = 3;

        const int MAX_MSG_SIZE = 100;

        const char COMMAND_EOL = '\0';
        const char COMMAND_DELIMITER = ';';
        const byte COMMAND_TRUCKNAMES  = 48;
        const byte COMMAND_VALIDPACKAGE = 49;
        const byte COMMAND_PACKETINFOS = 50;
        const byte COMMAND_SETPACKETSTATE = 51;
        const byte COMMAND_GETPACKAGES = 52;
        const byte COMMAND_GETMSGS = 53;
        const byte COMMAND_MSGFROMPDA = 54;
        const byte COMMAND_MSGTOPDA = 55;
        const byte COMMAND_GPSCOORD = 60;
        const byte COMMAND_GETMAP = 61;
        const int COMMAND_CODEBARLENGTH = 20;

        /* Socket global pour le PDA */
        static IPEndPoint ipep;
        static System.Net.Sockets.Socket newsock;
        static System.Net.Sockets.Socket client;

        //Semaphores
        static TestSemaphore.Semaphore msgSendSem;
        static TestSemaphore.Semaphore msgReceivedSem;
        
        //Buffer partages entre les threads
        static byte[] msgSendData = new byte[1024];
        static byte[] msgReceivedData = new byte[1024];
        static byte[] receivedData = new byte[1024];
        static byte[] webReceivedData = new byte[1024];
        static byte[] toSendData = new byte[1024];

        static DataManager m_DataManager;

        /*
        *********************************************************************************************************
        *                                              SendData()
        *
        * Description : Cette fonction envoie des données sur le socket
        *
        * Argument(s) : s               Le socket de communication
        *               data            Buffer contenant le data à envoyer
        *
        * Return(s)   : int             Le nombre d'octets envoyés sur le socket
        *********************************************************************************************************
        */
        private static int SendData(System.Net.Sockets.Socket s, byte[] data)
        {
            int total = 0;
            int size = data.Length;
            int dataleft = size;
            int sent;

            while (total < size)
            {
                sent = s.Send(data, total, dataleft, SocketFlags.None);
                total += sent;
                dataleft -= sent;
            }
            return total;
        }

        /*
        *********************************************************************************************************
        *                                              ReceiveData()
        *
        * Description : Cette fonction envoie du data sur le socket
        *
        * Argument(s) : s               Le socket de communication
        *               size            Le nombre d'octets à recevoir sur le socket
        *
        * Return(s)   : byte[]          Le tableau d'octets contenant les données reçues
        *********************************************************************************************************
        */
        private static byte[] ReceiveData(System.Net.Sockets.Socket s, int size)
        {
            int total = 0;
            int dataleft = size;
            byte[] data = new byte[size];
            int recv;

            while (total < size)
            {
                recv = s.Receive(data, total, dataleft, 0);
                if (recv == 0)
                {
                    data = Encoding.ASCII.GetBytes("exit ");
                    break;
                }
                total += recv;
                dataleft -= recv;
            }
            return data;
        }

		/*
        *********************************************************************************************************
        *                                              Main()
        *
        * Description : Fonction principale qui initialise plusieurs variables
        *
        * Argument(s) : aucun
        *
        * Return(s)   : aucun
        *********************************************************************************************************
        */
        public static void Main()
        {
        	// Semaphores qui vont synchroniser 2 taches chacune
            msgSendSem = new TestSemaphore.Semaphore(2);
            msgReceivedSem = new TestSemaphore.Semaphore(2);

			// Les buffers partages sont initialises au caractere de delimitation pour indiquer qu'ils sont vides
            msgSendData[0] = (byte) COMMAND_DELIMITER;
            msgReceivedData[0] = (byte) COMMAND_DELIMITER;

            m_DataManager = new DataManager();

			// Creation d'un socket TCP
            ipep = new IPEndPoint(IPAddress.Any, 2166);

            newsock = new System.Net.Sockets.Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            // Creation et demarrage de la tache qui interagit avec le serveur PC
            Thread WebThread = new Thread(new ThreadStart(TCPWeb));
            WebThread.Start();

            Console.WriteLine("Waiting for a PDA connexion on port 2166");
            client = newsock.Accept();
            IPEndPoint newclient = (IPEndPoint)client.RemoteEndPoint;
            Console.WriteLine("PDA Connected with {0} at port {1}", newclient.Address, newclient.Port);

			// Creation et demarrage de la tache qui interagit avec le PDA
            Thread ReceiverPdaThread = new Thread(new ThreadStart(TCPReceiverPDA));
            ReceiverPdaThread.Start();
        }

		/*
        *********************************************************************************************************
        *                                              TCPReceiverPDA()
        *
        * Description : Tache qui permet de communiquer avec le PDA. Elle recoit des numeros de commandes et envoit
        *				les informations appropriees au PDA. 
        *
        * Argument(s) : aucun
        *
        * Return(s)   : aucun
        *********************************************************************************************************
        */
        static private void TCPReceiverPDA()
        {
        	// On place le caractere de delimitation dans un tableau de caracteres
            char[] delimiter = new char[1];
            delimiter[0] = (char)COMMAND_DELIMITER;
            
            while (true)
            {
                try {
                	// Reception de la commande
                    receivedData = new byte[100];
                    client.Receive(receivedData);
                    Console.WriteLine("Client sent data");
                    Console.WriteLine("ASCII Data: " + Encoding.ASCII.GetString(receivedData));

					// Si c'est un message a acheminer au serveur PC
					if (receivedData[0] == COMMAND_MSGFROMPDA) {
                        Console.WriteLine("Received send message from pda command");
                        msgReceivedSem.Wait();
                        
                        // Si le buffer des messages recus n'est pas vide
                        if (msgReceivedData[0] != COMMAND_DELIMITER) {
                            // Separation des messages deja presents
                            string messageCat = "";
                            string[] message = Encoding.ASCII.GetString(msgReceivedData).Split(delimiter, 10);

                            for (int i = 0; i < message.Length; i++) {
                                if (message[i][0] != '\0') {
                                    messageCat += message[i] + ";";
                                    Console.WriteLine(message[i]);
                                }
                            }

							// Ajout du nouveau message dans le buffer des messages recus
                            messageCat += Encoding.ASCII.GetString(receivedData);
                            msgReceivedData = Encoding.ASCII.GetBytes(messageCat);
                        } 
                        //Si le buffer des messages recus est vide, on ajoute simplement le message
                        else {
                            string message = Encoding.ASCII.GetString(receivedData).Split(delimiter)[0] + ";";
                            Console.WriteLine("Message: " + message);
                            msgReceivedData = Encoding.ASCII.GetBytes(message);
                        }
                        msgReceivedSem.Release();
                    } 
                    // Si le PDA desire mettre a jour sa liste de messges recus du serveur
                    else if (receivedData[0] == COMMAND_GETMSGS) 
                    {
                        Console.WriteLine("Received get messages");
                        msgSendSem.Wait();

                        string messageCat = "";
                        string[] message = Encoding.ASCII.GetString(msgSendData).Split(delimiter, 5);
                        Console.WriteLine("Message encoded in ascii bytes");
                        
                        // Si le buffer n'est pas vide, on ajoute les messages dans messageCat
                        if (msgSendData[0] != COMMAND_DELIMITER) {
                            for (int i = 0; i < message.Length; i++) {
                                if (message[i][0] != '\0') {
                                    messageCat += message[i];
                                    Console.WriteLine("Adding message: " + message[i]);
                                }
                            }
                        }
                        
                        // Si le buffer est vide, on le signale a l'utilisateur
                        if (messageCat == string.Empty || messageCat == null) {
                            messageCat = "No messages\0";
                        }
                        
                        // Envoi au PDA de la string messageCat, qui contient soit les 5 messages les plus recents ou la strin "No messages"
                        Console.WriteLine("MessageCat built: " + messageCat);
                        byte[] tempBuffer = new byte[messageCat.Length];
                        tempBuffer = Encoding.ASCII.GetBytes(messageCat);

                        client.Send(tempBuffer);
                        msgSendSem.Release();
                    }
                    // Si le PDa desire avoir les noms des camions
                    else if (receivedData[0] == COMMAND_TRUCKNAMES)
                    {
                        Console.WriteLine("Received get truck names command");
                        string listeCamions = DataManager.GetTruckList();
                       
                        toSendData = new byte[listeCamions.Length];
                        toSendData = Encoding.ASCII.GetBytes(listeCamions);
                        client.Send(toSendData);
                    } 
                    // Si le PDA veut valider l'existence d'un colis dans la base de donnees
                    else if (receivedData[0] == COMMAND_VALIDPACKAGE) 
                    {
                        Console.WriteLine("Received isvalid package command");
                        string command = Encoding.ASCII.GetString(receivedData);
                        string package = DataManager.GetColis(command.Substring(1, COMMAND_CODEBARLENGTH));
                        toSendData = new byte[3];
                        toSendData[0] = (byte) COMMAND_VALIDPACKAGE;
                        if (package == "")
                        {
                            toSendData[1] = (byte) 0;
                        }
                        else
                        {
                            toSendData[1] = (byte) 1;
                        }
                        toSendData[2] = (byte) COMMAND_DELIMITER;
                        client.Send(toSendData);
                    } 
                    // Si le PDA veut changer l'etat d'un colis
                    else if (receivedData[0] == COMMAND_SETPACKETSTATE) 
                    {
                        Console.WriteLine("Received set package state command");
                        string command = Encoding.ASCII.GetString(receivedData);
                        DataManager.SaveEtatColis(command.Substring(1, COMMAND_CODEBARLENGTH), command.Substring(COMMAND_CODEBARLENGTH + 1, 1));
                        Console.WriteLine("The package {0} had its state set to {1}", command.Substring(1, COMMAND_CODEBARLENGTH), command.Substring(COMMAND_CODEBARLENGTH + 1, 1));

                    } 
                    // Si le PDA veut avoir la liste des colis qui lui sont attribues
                    else if (receivedData[0] == COMMAND_GETPACKAGES) 
                    {
                        Console.WriteLine("Received get packages command");
                        string command = Encoding.ASCII.GetString(receivedData);
                        string packages = DataManager.GetColisList(command.Substring(1, 1));

                        toSendData = new byte[packages.Length];
                        toSendData = Encoding.ASCII.GetBytes(packages);
                        client.Send(toSendData);
                    }
                    // Si le PDA veut lire les informations d'un colis en particulier
                    else if (receivedData[0] == COMMAND_PACKETINFOS) 
                    {
                        Console.WriteLine("Received Packet infos command");
                        string command = Encoding.ASCII.GetString(receivedData);
                        string package = DataManager.GetColis(command.Substring(1, COMMAND_CODEBARLENGTH));
                        toSendData = new byte[package.Length];
                        toSendData = Encoding.ASCII.GetBytes(package);
                        client.Send(toSendData);
                    } 
                    // Si le PDA veut envoyer ses coordonnees GPS mises a jour
                    else if (receivedData[0] == COMMAND_GPSCOORD) {
                        Console.WriteLine("Received GPS update command");
                        string command = Encoding.ASCII.GetString(receivedData);
                        string[] coord = command.Substring(2).Split(';');
                        if (coord.Length >= 2) {
                            m_DataManager.GpsData[0].Longitude = double.Parse(coord[0]);
                            m_DataManager.GpsData[0].Latitude = double.Parse(coord[1]);
                            Console.WriteLine("Latitude updated: " + m_DataManager.GpsData[0].Latitude);
                            Console.WriteLine("Longitude updated: " + m_DataManager.GpsData[0].Longitude);
                        } else {
                            Console.WriteLine("incomplete data informations");
                        }

                    }
                    // Si le PDA veut obtenir une nouvelle carte
                    else if (receivedData[0] == COMMAND_GETMAP) {
                        MemoryStream stream = m_DataManager.GetCurrentMap();
                        if (stream != null) {
                            byte[] maparray = new byte[stream.Length+1];
                            stream.Position = 0;
                            stream.Read(maparray, 0, (int)stream.Length);
                            client.Send(maparray);
                        }
                    } else {
                        Console.WriteLine("X-----Invalid message tag (TCPReceiverPDA)-----X\n");
                    }
                } catch (SocketException) { 
                    // PDA Disconnected
                    client = newsock.Accept();
                }
            }
        }

		/*
        *********************************************************************************************************
        *                                              TCPWeb()
        *
        * Description : Tache qui permet de communiquer avec l'interface du serveur PC. Elle peut recevoir un
        *               message en provenance du serveur afin qu'il soit envoye au PDA, ou bien recevoir une demande
        *				pour mettre a jour les messages affiches sur l'interface      
        *  
        * Argument(s) : aucun
        *
        * Return(s)   : aucun
        *********************************************************************************************************
        */
        static private void TCPWeb()
        {
        	// Creation d'un socket TCP pour ecouter les requetes en provenance du serveur PC
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 2160);

            System.Net.Sockets.Socket newsock = new System.Net.Sockets.Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a WEB connexion on port 2160");

            System.Net.Sockets.Socket client;
            IPEndPoint newclient;

			// On place le caractere de delimitation dans un tableau de caracteres
            char[] delimiter = new char[1];
            delimiter[0] = (char)COMMAND_DELIMITER;

            while (true)
            {
            	// On accepte un connexion venant du serveur
                client = newsock.Accept();
                Console.WriteLine("WEB Connected with {0} at port {1}", ((IPEndPoint)client.RemoteEndPoint).Address, ((IPEndPoint)client.RemoteEndPoint).Port);
                webReceivedData = new byte[1024];                

                if (client != null)
                {
                    newclient = (IPEndPoint)client.RemoteEndPoint;
                    client.Receive(webReceivedData);
                    
                    // Si le serveur veut mettre a jour les messages qu'il affiche dans son interface
                    if (webReceivedData[0] == COMMAND_GETMSGS)
                    {
                        Console.WriteLine("Received command get messages from WEB GUI");
                        msgReceivedSem.Wait();

						// Si le buffer des messages recus n'est pas vide
                        if (msgReceivedData[0] != COMMAND_DELIMITER)
                        {
                        	// On verifie que le buffer contient bien un/des messages venant du PDA. Si oui, on envoit le buffer au serveur
                            if (msgReceivedData[0] == COMMAND_MSGFROMPDA)
                            {
                                Console.WriteLine(Encoding.ASCII.GetString(msgReceivedData));
                                client.Send(msgReceivedData);
                                msgReceivedData = new byte[1024];
                                msgReceivedData[0] = (byte) COMMAND_DELIMITER;
                            }
                            else
                            {
                                Console.WriteLine("X-----Memory probably corrupted (TCPWeb)-----X");                                
                            }                             
                        }

                        msgReceivedSem.Release();                            
                    }
                    // Si le serveur veut envoyer un message au PDA
                    else if (webReceivedData[0] == COMMAND_MSGTOPDA)
                    {
                        Console.WriteLine("Received command send messages to PDA from WEB GUI");
                        msgSendSem.Wait();

						// Si le buffer des messages a envoyer n'est pas vide
                        if (msgSendData[0] != COMMAND_DELIMITER)
                        {
                        	// On separe les messages deja presents et on ajoute le nouveau au debut du buffer
                            string messageCat = "";
                            string[] message = Encoding.ASCII.GetString(msgSendData).Split(delimiter, 5);

                            for (int i = 0; i < message.GetLength(0); i++)
                            {
                                if (message[i][0] != '\0')
                                {
                                    messageCat = message[i] + "\n\r" + messageCat;
                                }
                            }

							// On ajoute le nouveau message dans le buffer d'envoi
                            string newMessage = Encoding.ASCII.GetString(webReceivedData);
                            messageCat += newMessage.Substring(1, newMessage.Length - 1);
                            msgSendData = Encoding.ASCII.GetBytes(messageCat);
                        }
                        // Si le buffer est vide, on ajoute simplement le message dans le buffer d'envoi
                        else
                        {
                            string message = Encoding.ASCII.GetString(webReceivedData);
                            msgSendData = Encoding.ASCII.GetBytes(message.Substring(1, message.Length - 1));
                        }

                        msgSendSem.Release();
                    }
                    else
                    {
                        Console.WriteLine("X-----Invalid message tag (TCPWeb)-----X\n");
                    }

                }
                client.Close();
            }
        }
    }
}
