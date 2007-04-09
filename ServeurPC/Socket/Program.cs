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

            SenderPdaThread.Start();
            ReceiverPdaThread.Start();

        


           

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
    }
}
