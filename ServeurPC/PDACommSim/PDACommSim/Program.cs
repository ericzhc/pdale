using System;
using System.IO;
using System.Drawing;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;

namespace PDACommSim
{
    class Program
    {
        private const byte COMMAND_EOL = 59;
        private const byte COMMAND_TRUCKNAMES = 48;
        private const byte COMMAND_VALIDPACKAGE = 49;
        private const byte COMMAND_PACKETINFOS = 50;
        private const byte COMMAND_SETPACKETSTATE = 51;
        private const byte COMMAND_GETPACKAGES = 52;
        private const byte COMMAND_GETMSGS = 53;
        private const byte COMMAND_SENDMSG = 54;
        private const byte COMMAND_GPSCOORD = 60;
        private const byte COMMAND_GETMAP = 61;

        private const string MARC_IP = "127.0.0.1";
        private const int MARC_PORT = 2166;
        
        private const string menu = @"MENU - MAKE SELECTION
        1 - GetTruckNames
        2 - IsValidPackage
        3 - GetPacketInfos
        4 - SetPacketState
        5 - GetAllPackages
        6 - GetMessages
        7 - SendMessage
        8 - GetMap (saved in c:\Map.jpg)
        Selection:";

        private static Socket sock;

        static void Main(string[] args)
        {
            sock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            sock.Connect(MARC_IP, MARC_PORT);
            string input;

            while(true) {
                Console.Out.Write(menu);
                input = Console.In.ReadLine();
                switch(input) {
                    case "1":
                        GetTruckNames();
                        break;
                    case "2":
                        IsValidPackage();
                        break;
                    case "3":
                        GetPacketInfos();
                        break;
                    case "4":
                        SetPacketState();
                        break;
                    case "5":
                        GetAllPackages();
                        break;
                    case "6":
                        GetMessages();
                        break;
                    case "7":
                        SendMessage();
                        break;
                    case "8":
                        GetMap();
                        break;
                    default:
                        Console.Out.WriteLine("Invalid menu selection");
                        break;
                }
            }
        }

        private static void GetTruckNames() 
        {
            byte[] data = {COMMAND_TRUCKNAMES, COMMAND_EOL};

            // Send command
            sock.Send(data);
            // Wait for response
            byte[] buffer = new byte[2000];
            sock.Receive(buffer);

            Console.WriteLine("Received: {0}", buffer);
        }

        private static void IsValidPackage()
        {

        }

        private static void GetPacketInfos()
        {

        }

        private static void SetPacketState()
        {

        }

        private static void GetAllPackages()
        {

        }

        private static void GetMessages()
        {
            Console.WriteLine("Getting messages...");
            byte[] data = new byte[2];
            data[0] = COMMAND_GETMSGS;
            data[1] = COMMAND_EOL;

            char[] delimiter = new char[1];
            delimiter[0] = (char) COMMAND_EOL;

            sock.Send(data);
            
            byte[] messages = new byte[1024];
            int msgLength = sock.Receive(messages);

            string[] strMessage = Encoding.ASCII.GetString(messages).Split(delimiter, 10);

            for (int i = 0; i < strMessage.GetLength(0); i++)
            {
                if (strMessage[i][0] != '\0')
                {
                    Console.WriteLine(strMessage[i]);
                }
            }
        }

        private static void SendMessage()
        {
            Console.Out.Write("Message:");
            byte[] message = System.Text.Encoding.ASCII.GetBytes(Console.In.ReadLine());
            byte[] data = new byte[message.Length + 3];
            int i;
	        data[0] = COMMAND_SENDMSG;
            data[1] = 49;   

            for (i = 0; i < message.Length; i++) {
		        data[i+2] = message[i];
	        }
	        data[i+2] = COMMAND_EOL;

            sock.Send(data);
            Console.WriteLine("Sent messages");
        }

        private static void GetMap()
        {
            Console.WriteLine("Get map");
            byte[] data = new byte[2];
            data[0] = COMMAND_GETMAP;
            data[1] = COMMAND_EOL;
            sock.Send(data);

            char[] delimiter = new char[1];
            delimiter[0] = (char)COMMAND_EOL;

            byte[] image = new byte[50 * 1024];
            byte[] bufftemp = new byte[2048];
            int imageLength = 0, compteur = 0;
            sock.ReceiveTimeout = 1000;
            while (true) {
                try {
                    imageLength = sock.Receive(bufftemp);
                    if (imageLength == 0) {
                        break;
                    } else {
                        bufftemp.CopyTo(image, compteur);
                    }
                    compteur += imageLength;
                } catch (SocketException exp) {
                    Console.WriteLine("Map received");
                    break;
                }
            }

            Stream stream = new MemoryStream(image);
            Bitmap bitmap = new Bitmap(stream);
            bitmap.Save("c:\\map.jpg", System.Drawing.Imaging.ImageFormat.Jpeg);
            //string[] strMessage = Encoding.ASCII.GetString(messages).Split(delimiter, 10);
        }
    }
}
