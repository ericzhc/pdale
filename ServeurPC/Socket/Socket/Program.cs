using MySql.Data.MySqlClient;
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

        private static MySqlConnection m_SqlConnection;
        private string connectionString =
                "Server=69.16.250.95;" +
                "Database=pdale;" +
                "User ID=pdale;" +
                "Password=projets5;";

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
                if (client != null)
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

        // Retourne les champs d'un colis séparés par un \0 dans un string
        public string GetColis(string str_ColIdent)
        {
            try
            {
                string str_Sql = "";
                string str_DonneesColis = "";
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;
                MySqlDataReader MyReader = null;

                str_Sql = "SELECT * FROM colis WHERE col_noident='" + str_ColIdent + "'";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                while (MyReader.Read())
                {
                    str_DonneesColis = MyReader[1].ToString() + "\0";
                    str_DonneesColis += MyReader[2].ToString() + "\0";
                    str_DonneesColis += MyReader[3].ToString() + "\0";
                    str_DonneesColis += MyReader[4].ToString() + "\0";
                    str_DonneesColis += MyReader[5].ToString() + "\0";
                    str_DonneesColis += MyReader[6].ToString() + "\0";
                    str_DonneesColis += MyReader[7].ToString() + "\0";
                    str_DonneesColis += MyReader[8].ToString() + "\0";
                    str_DonneesColis += MyReader[9].ToString() + "\0";
                    str_DonneesColis += MyReader[9].ToString() + "\0";
                    str_DonneesColis += MyReader[10].ToString() + "\0";
                    str_DonneesColis += MyReader[11].ToString() + "\0";
                    str_DonneesColis += MyReader[12].ToString() + "\0";
                    str_DonneesColis += MyReader[13].ToString() + "\0";
                    str_DonneesColis += MyReader[14].ToString() + "\0";
                    str_DonneesColis += MyReader[15].ToString();
                }
                MyReader.Close();
                return str_DonneesColis;
            }
            catch (MySqlException myEx)
            {
                return "";
            }
        }

        // Retourne le nombre de camion
        public string GetNbrCamion()
        {
            try
            {
                string str_Sql = "";
                string str_nbrCamion = "";
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;
                MySqlDataReader MyReader = null;

                str_Sql = "SELECT COUNT(*) FROM camion";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                if (MyReader.Read())
                {
                    str_nbrCamion = MyReader[0].ToString();
                }
                MyReader.Close();
                return str_nbrCamion;
            }
            catch (MySqlException myEx)
            {
                return "";
            }
        }

        // Retourne le nom du camion correspondant à l'index dans la bd
        public string GetNomFromIndex(string str_IdCamion)
        {
            try
            {
                string str_Sql = "";
                string str_NomCamion = "";
                int compteur = 0;
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;
                MySqlDataReader MyReader = null;

                str_Sql = "SELECT cam_nom FROM camion";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                while (MyReader.Read())
                {
                    compteur++;
                    if (compteur.ToString() == str_IdCamion)
                    {
                        str_NomCamion = MyReader[0].ToString();
                    }
                }
                MyReader.Close();
                return str_NomCamion;
            }
            catch (MySqlException myEx)
            {
                return "";
            }
        }

        // Modifie l'etat d'un colis dans la BD
        public void SaveEtatColis(string str_IdColis, string str_EtatColis)
        {
            try
            {
                string str_Sql = "";
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;

                str_Sql = "UPDATE colis SET col_etat = '" + str_EtatColis + "' WHERE str_IdColis = '" + str_IdColis + "'";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyCommand.EndExecuteNonQuery();
            }
            catch (MySqlException myEx)
            {
            }
        }

        // Retourne la liste de colis et de leur etat pour un camion donné
        public string GetColisList(string str_NomCamion)
        {
            try
            {
                string str_Sql = "";
                string str_ColisList = "";
                int compteur = 0;
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;
                MySqlDataReader MyReader = null;

                str_Sql = "SELECT col_noident, col_etat FROM colis WHERE cam_nom='" + strNomCamion + "'";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                while (MyReader.Read())
                {
                    str_ColisList += MyReader[0].ToString() + "\0";
                    str_ColisList += MyReader[1].ToString() + "\0";
                }
                MyReader.Close();
                return str_ColisList;
            }
            catch (MySqlException myEx)
            {
                return "";
            }
        }

        public static MySqlConnection GetConnection()
        {
            if (m_SqlConnection == null)
            {
                m_SqlConnection = new MySqlConnection();
            }
            if (m_SqlConnection.State == ConnectionState.Closed)
            {
                m_SqlConnection.ConnectionString = str_ConnString;
                m_SqlConnection.Open();
            }

            return m_SqlConnection;
        }
    }
}
