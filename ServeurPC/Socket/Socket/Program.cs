/*
*********************************************************************************************************
* Fichier : Program.cs
* Par     : Marc-Étienne Lebeau, Julien Beaumier-Ethier, Richard Labonté
* Date    : 2007/04/12
*********************************************************************************************************
*/
using Socket.MapPoint;
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

        /* Socket global pour le PDA */
        static IPEndPoint ipep;
        static System.Net.Sockets.Socket newsock;
        static System.Net.Sockets.Socket client;

        //Semaphores
        static TestSemaphore.Semaphore msgSendSem;
        static TestSemaphore.Semaphore msgReceivedSem;
        static TestSemaphore.Semaphore sendSem;
        //static TestSemaphore.Semaphore recvSem;

        //static string[] messagesReceived = new string[5];
        //static string[] messagesToSend = new string[100];

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

        public static void Main()
        {
            msgSendSem = new TestSemaphore.Semaphore(2);
            msgReceivedSem = new TestSemaphore.Semaphore(2);
            sendSem = new TestSemaphore.Semaphore(2);

            msgSendData[0] = (byte) COMMAND_DELIMITER;
            msgReceivedData[0] = (byte) COMMAND_DELIMITER;
            toSendData[0] = (byte) COMMAND_DELIMITER;

            m_DataManager = new DataManager();

            ipep = new IPEndPoint(IPAddress.Any, 2166);

            newsock = new System.Net.Sockets.Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            // Start the web thread that handles a connection with the web GUI
            Thread WebThread = new Thread(new ThreadStart(TCPWeb));
            WebThread.Start();

            client = newsock.Accept();
            IPEndPoint newclient = (IPEndPoint)client.RemoteEndPoint;
            Console.WriteLine("Connected with {0} at port {1}", newclient.Address, newclient.Port);

            Thread ReceiverPdaThread = new Thread(new ThreadStart(TCPReceiverPDA));
            ReceiverPdaThread.Start();
            
            // Centrale default addreseses ?
            //string OrigAddress = "1408 RUE DE L'EGLISE;SAINT-LAURENT;QC;H4L2H3";
            //string DestAddress = "8105 BOULEVARD DECARIE;MONTREAL;QC;H4P2H5";

            //GetDirectionsFromAdress(OrigAddress, DestAddress);
            //GetRouteFromGps(45.40391131, -71.88929146, 46.34018682, -72.54498962);
        }

        static private void TCPReceiverPDA()
        {
            char[] delimiter = new char[1];
            delimiter[0] = (char)COMMAND_DELIMITER;
            
            while (true)
            {
                receivedData = new byte[1024];
                client.Receive(receivedData);
                
                if (receivedData[0] == COMMAND_MSGFROMPDA)
                {
                    msgReceivedSem.Wait();
                    if (msgReceivedData[0] != COMMAND_DELIMITER)
                    {
                        string messageCat = "";
                        string[] message = Encoding.ASCII.GetString(msgReceivedData).Split(delimiter, 10);

                        for (int i = 0; i < message.GetLength(0); i++)
                        {
                            if (message[i][0] != '\0')
                            {
                                messageCat += message[i] + ";";
                            }
                        }

                        messageCat += Encoding.ASCII.GetString(receivedData);
                        msgReceivedData = Encoding.ASCII.GetBytes(messageCat);
                    }
                    else
                    {
                        string message = Encoding.ASCII.GetString(receivedData);
                        msgReceivedData = Encoding.ASCII.GetBytes(message);
                    }
                    msgReceivedSem.Release();
                }
                else if (receivedData[0] == COMMAND_GETMSGS)
                {
                    msgSendSem.Wait();
                    
                    string messageCat = "";
                    string[] message = Encoding.ASCII.GetString(msgSendData).Split(delimiter, 10);

                    for (int i = 0; i < message.GetLength(0); i++)
                    {
                        if (message[i][0] != '\0')
                        {
                            messageCat += message[i] + ";";
                        }
                    }

                    byte[] tempBuffer = new byte[messageCat.Length];
                    tempBuffer = Encoding.ASCII.GetBytes(messageCat);

                    client.Send(tempBuffer);
                    msgSendData = new byte[1024];
                    msgSendSem.Release();

                }
                else if (receivedData[0] == COMMAND_TRUCKNAMES)
                {
                    string listeCamions = GetCamionList();

                    sendSem.Wait();
                    if (toSendData[0] != COMMAND_DELIMITER)
                    {
                        string data = toSendData.ToString() + listeCamions;
                        toSendData = Encoding.ASCII.GetBytes(data);
                    }
                    else
                    {
                        toSendData = Encoding.ASCII.GetBytes(strReceivedData[idxStr]);
                    }
                    sendSem.Release();
                }
                else if (receivedData[0] == COMMAND_VALIDPACKAGE)
                {

                }
                else if (receivedData[0] == COMMAND_SETPACKETSTATE)
                {

                }
                else if (receivedData[0] == COMMAND_GETPACKAGES)
                {

                }
                else if (receivedData[0] == COMMAND_PACKETINFOS)
                {

                }
                else if (receivedData[0] == COMMAND_GPSCOORD) 
                {
                    
                }
                else if (receivedData[0] == COMMAND_GETMAP) 
                {
                    
                }
                else
                {
                    Console.WriteLine("X-----Invalid message tag (TCPReceiverPDA)-----X\n");
                }
            }
        }

        static private void TCPWeb()
        {

            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 2160);

            System.Net.Sockets.Socket newsock = new System.Net.Sockets.Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a connexion on port 2160");

            System.Net.Sockets.Socket client;
            IPEndPoint newclient;

            char[] delimiter = new char[1];
            delimiter[0] = (char)COMMAND_DELIMITER;

            while (true)
            {
                client = newsock.Accept();
                
                webReceivedData = new byte[1024];                

                if (client != null)
                {
                    newclient = (IPEndPoint)client.RemoteEndPoint;
                    client.Receive(webReceivedData);
                    
                    if (webReceivedData[0] == COMMAND_GETMSGS)
                    {
                        msgReceivedSem.Wait();

                        if (msgReceivedData[0] != COMMAND_DELIMITER)
                        {
                            if (msgReceivedData[0] == COMMAND_MSGFROMPDA)
                            {   
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
                    else if (webReceivedData[0] == COMMAND_MSGTOPDA)
                    {
                        msgSendSem.Wait();

                        if (msgSendData[0] != COMMAND_DELIMITER)
                        {
                            string messageCat = "";
                            string[] message = Encoding.ASCII.GetString(msgSendData).Split(delimiter, 10);

                            for (int i = 0; i < message.GetLength(0); i++)
                            {
                                if (message[i][0] != '\0')
                                {
                                    messageCat += message[i] + ";";
                                }
                            }   
                            
                            messageCat += Encoding.ASCII.GetString(webReceivedData);
                            msgSendData = Encoding.ASCII.GetBytes(messageCat);
                        }
                        else
                        {
                            string message = Encoding.ASCII.GetString(webReceivedData);
                            msgSendData = Encoding.ASCII.GetBytes(message);
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
