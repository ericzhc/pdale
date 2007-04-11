using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace TCPServerReceiver
{
    class Program
    {
        static string[] messagesReceived = new string[5];
        static string[] messagesToSend = new string[100];


        private static int SendData(Socket s, byte[] data)
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

        private static byte[] ReceiveData(Socket s, int size)
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
            Thread SenderPdaThread = new Thread(new ThreadStart(TCPSenderPDA));
            Thread ReceiverPdaThread = new Thread(new ThreadStart(TCPReceiverPDA));
            Thread WebThread = new Thread(new ThreadStart(TCPWeb));

            SenderPdaThread.Start();
            ReceiverPdaThread.Start();
            WebThread.Start();

            messagesReceived[0] = "Salut marc voici le premier message;";
            messagesReceived[1] = "Salut marc voici le 222deuxieme message;";
            messagesReceived[2] = "Salut marc voici le troisieme message;";
            messagesReceived[3] = "Salut marc voici le quatrieme message;";
            messagesReceived[4] = "Salut marc voici le sixieme message (mais non cetait le cinqueieme hihihihi);";



        }
        static private void TCPSenderPDA()
        {
            byte[] data = new byte[1024];
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 2165);

            Socket newsock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a connexion on port 2165");

            Socket client = newsock.Accept();
            IPEndPoint newclient = (IPEndPoint)client.RemoteEndPoint;
            Console.WriteLine("Connected with {0} at port {1}", newclient.Address, newclient.Port);


            while (true)
            {
                Console.WriteLine("\nMessage to send: ");
                string msgSend = Console.ReadLine();
                data = Encoding.ASCII.GetBytes(msgSend);
                client.Send(data);
            }
        }

        static private void TCPReceiverPDA()
        {
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 2166);

            Socket newsock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a connexion on port 2166");

            Socket client = newsock.Accept();
            IPEndPoint newclient = (IPEndPoint)client.RemoteEndPoint;
            Console.WriteLine("Connected with {0} at port {1}", newclient.Address, newclient.Port);


            byte[] data = new byte[1024];
            while (true)
            {
                client.Receive(data);
                string strData = Encoding.ASCII.GetString(data);
                strData = strData.Remove(strData.IndexOf("\0"));
                Console.WriteLine("Message received: " + strData + "\n");
                data = new byte[1024];
            }
        }

        static private void TCPWeb()
        {

            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 2160);

            Socket newsock = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a connexion on port 2160");

            Socket client;
            IPEndPoint newclient;
            
            byte[] data = new byte[1024];
            while (true)
            {
                client = newsock.Accept();
                if(client != null)
                {
                    newclient = (IPEndPoint)client.RemoteEndPoint;
                    Console.WriteLine("Connected with {0} at port {1}", newclient.Address, newclient.Port);
                    client.Receive(data);
                    string strData = Encoding.ASCII.GetString(data);
                    strData = strData.Remove(strData.IndexOf("\0"));
                    Console.WriteLine("Message received: " + strData + "\n");
                    if (strData == "update")
                    {
                        int i = 0;
                        data = new byte[1024];
                        while ((i < 5) && (messagesReceived[i] != null))
                        {

                            data = Encoding.ASCII.GetBytes(messagesReceived[i]);
                            client.Send(data);
                            //messagesReceived[i] = "";
                            i++;
                            data = new byte[1024];
                        }
                    }
                }
                client.Close();
            }

        }
    }
}
