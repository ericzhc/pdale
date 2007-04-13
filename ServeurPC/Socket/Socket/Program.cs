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

namespace TCPServerReceiver
{
    class Program
    { 
        /* Socket global pour le PDA */
        static IPEndPoint ipep;
        static System.Net.Sockets.Socket newsock;
        static System.Net.Sockets.Socket client;

        private static Semaphore dataAvailable;
        static string[] messagesReceived = new string[5];
        static string[] messagesToSend = new string[100];

        private static MySqlConnection m_SqlConnection;
        private static string str_ConnString =
                "Server=69.16.250.95;" +
                "Database=pdale;" +
                "User ID=pdale;" +
                "Password=projets5;";

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
            /*dataAvailable = new Semaphore(0, 1);

            ipep = new IPEndPoint(IPAddress.Any, 2166);

            newsock = new System.Net.Sockets.Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            client = newsock.Accept();
            IPEndPoint newclient = (IPEndPoint)client.RemoteEndPoint;
            Console.WriteLine("Connected with {0} at port {1}", newclient.Address, newclient.Port);

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
            */

            string OrigAddress = "1408 RUE DE L'EGLISE;SAINT-LAURENT;QC;H4L2H3";
            string DestAddress = "8105 BOULEVARD DECARIE;MONTREAL;QC;H4P2H5";

            //GetDirectionsFromAdress(OrigAddress, DestAddress);
            GetRouteFromGps(45.40391131, -71.88929146, 46.34018682, -72.54498962);
        }

        static private void TCPSenderPDA()
        {


            byte[] data = new byte[1024];


            while (true)
            {
                dataAvailable.WaitOne();
                //data = Encoding.ASCII.GetBytes(msgSend);
                client.Send(data);
                dataAvailable.Release();
            }
        }

        static private void TCPReceiverPDA()
        {

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a connexion on port 2166");


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

            System.Net.Sockets.Socket newsock = new System.Net.Sockets.Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            newsock.Bind(ipep);
            newsock.Listen(10);

            Console.WriteLine("Waiting for a connexion on port 2160");

            System.Net.Sockets.Socket client;
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

        /*
        *********************************************************************************************************
        *                                              GetColis()
        *
        * Description : Cette fonction retourne les champs d'un colis séparés par un ";" dans un string
        *
        * Argument(s) : str_ColIdent    Le numéro d'identification du colis
        *
        * Return(s)   : string          La variable string contenant les champs d'un colis
        *********************************************************************************************************
        */
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
                    str_DonneesColis = MyReader[1].ToString() + ";";
                    str_DonneesColis += MyReader[2].ToString() + ";";
                    str_DonneesColis += MyReader[3].ToString() + ";";
                    str_DonneesColis += MyReader[4].ToString() + ";";
                    str_DonneesColis += MyReader[5].ToString() + ";";
                    str_DonneesColis += MyReader[6].ToString() + ";";
                    str_DonneesColis += MyReader[7].ToString() + ";";
                    str_DonneesColis += MyReader[8].ToString() + ";";
                    str_DonneesColis += MyReader[9].ToString() + ";";
                    str_DonneesColis += MyReader[9].ToString() + ";";
                    str_DonneesColis += MyReader[10].ToString() + ";";
                    str_DonneesColis += MyReader[11].ToString() + ";";
                    str_DonneesColis += MyReader[12].ToString() + ";";
                    str_DonneesColis += MyReader[13].ToString() + ";";
                    str_DonneesColis += MyReader[14].ToString() + ";";
                    str_DonneesColis += MyReader[15].ToString() + "*";;
                }
                MyReader.Close();
                return str_DonneesColis;
            }
            catch (MySqlException myEx)
            {
                return "";
            }
        }

        /*
        *********************************************************************************************************
        *                                              GetNbrCamion()
        *
        * Description : Cette fonction retourne le nombre de camion contenu dans la BD
        *        
        * Return(s)   : string          Le nombre de camion contenu dans la BD
        *********************************************************************************************************
        */
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

        /*
        *********************************************************************************************************
        *                                              GetNomFromIndex()
        *
        * Description : Cette fonction retourne le nom du camion correspondant à un index dans la BD
        *
        * Argument(s) : str_IdCamion    L'index du camion
        *
        * Return(s)   : string          Le nom du camion
        *********************************************************************************************************
        */
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

        /*
        *********************************************************************************************************
        *                                              SaveEtatColis()
        *
        * Description : Cette fonction modifie l'état d'un colis dans la BD
        *
        * Argument(s) : str_IdColis    Le numéro d'identification du colis
        *               str_EtatColis  Le nouvel état du colis 
        *
        *********************************************************************************************************
        */
        public void SaveEtatColis(string str_IdColis, string str_EtatColis)
        {
            try
            {
                string str_Sql = "";
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;

                str_Sql = "UPDATE colis SET col_etat = '" + str_EtatColis + "' WHERE str_IdColis = '" + str_IdColis + "'";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyCommand.ExecuteNonQuery();
            }
            catch (MySqlException myEx)
            {
            }
        }

        /*
        *********************************************************************************************************
        *                                              GetColis()
        *
        * Description : Cette fonction retourne la liste des colis et leur état pour un camion donné
        *
        * Argument(s) : str_NomCamion   Le nom du camion
        *
        * Return(s)   : string          La liste des colis et de leur état tous séparés par un ";"
        *********************************************************************************************************
        */
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

                str_Sql = "SELECT col_noident, col_etat FROM colis WHERE cam_nom='" + str_NomCamion + "'";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                while (MyReader.Read())
                {
                    str_ColisList += MyReader[0].ToString() + ";";
                    str_ColisList += MyReader[1].ToString() + ";";
                }
                str_ColisList += "*";
                MyReader.Close();
                return str_ColisList;
            }
            catch (MySqlException myEx)
            {
                return "";
            }
        }

        /*
        *********************************************************************************************************
        *                                              GetConnection()
        *
        * Description : Cette fonction établit une connexion avec la base de données.
        *
        * Retourne    : MySqlConnection        La connexion avec la base de données MySql
        *********************************************************************************************************
        */
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

        /*
        *********************************************************************************************************
        *                                              GetColis()
        *
        * Description : Cette fonction effectue un appel au service web MapPoint afin de rechercher deux 
        *               adresses dans le but d'éventuellement tracer un trajet dans une image
        *
        * Argument(s) : strOrigAddress  L'adresse de départ
        *               strDestAddress  L'adresse de la destination 
        * 
        * Note        : Le format du string à passer en argument à la fonction est celui-ci:
        *               "ADRESSE;VILLE;PROVINCE;CODE POSTAL;PAYS"  
        *               ex: strOrigAddress = "1408 RUE DE L'EGLISE;SAINT-LAURENT;QC;H4L2H3;CA";
        *********************************************************************************************************
        */
        public static void GetDirectionsFromAdress(string strOrigAddress, string strDestAddress)
        {
            FindServiceSoap findService = new FindServiceSoap();
            findService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
            FindSpecification findSpec = new FindSpecification();
            FindResults startResults = null;
            FindResults endResults = null;


            //Output the formatted address
            string [] strTemp = new String[5];
            strTemp = strOrigAddress.Split(';');

            Address myOrigAddress = new Address();
            myOrigAddress.AddressLine = strTemp[0];
            myOrigAddress.PrimaryCity = strTemp[1];
            myOrigAddress.Subdivision = strTemp[2];
            myOrigAddress.PostalCode = strTemp[3];
            myOrigAddress.CountryRegion = strTemp[4];

            FindAddressSpecification findOrigAddressSpec = new FindAddressSpecification();
            findOrigAddressSpec.InputAddress = myOrigAddress;
            findOrigAddressSpec.DataSourceName = "MapPoint.NA";

            //Retrieve the values of startResults
            try
            {
                startResults = findService.FindAddress(findOrigAddressSpec);
            }
            catch (Exception e2)  //The request failed with HTTP status 401: Unauthorized.
            {
                Console.WriteLine("Problem connecting with service");
            }

            //Output the formatted address
            strTemp = strDestAddress.Split(';');

            Address myDestAddress = new Address();
            myDestAddress.AddressLine = strTemp[0];
            myDestAddress.PrimaryCity = strTemp[1];
            myDestAddress.Subdivision = strTemp[2];
            myDestAddress.PostalCode = strTemp[3];
            myDestAddress.CountryRegion = strTemp[4];

            FindAddressSpecification findDestAddressSpec = new FindAddressSpecification();
            findDestAddressSpec.InputAddress = myDestAddress;
            findDestAddressSpec.DataSourceName = "MapPoint.NA";

            //Retrieve the values of endResults
            try
            {
                endResults = findService.FindAddress(findDestAddressSpec);
            }
            catch
            {
                Console.WriteLine("Problem connecting with service");
            }

            // Make sure findResults isn't null
            if (startResults == null)
            {
                Console.WriteLine("Originating Address not found.");
            }
            else
            {
                // If no results were found, display error and return
                if (startResults.NumberFound == 0)
                {
                    Console.WriteLine("Originating Address not found.");
                    return;
                }
            }
            if (endResults == null)
            {
                Console.WriteLine("Destination Address not found.");
            }
            else
            {
                // If no results were found, display error and return
                if (endResults.NumberFound == 0)
                {
                    Console.WriteLine("Destination Address not found.");
                    return;
                }
            }

            //Call GetRoute:Calculates the Route and also generates the Map
            GetRoute(startResults, endResults);
        }

        //GetRoute:Calculates the Route and 
        //also generates the Map
        /*
        *********************************************************************************************************
        *                                              GetColis()
        *
        * Description : Cette fonction effectue le trajet et génère la carte dans un fichier bitmap.
        *
        * Argument(s) : sResults        Le résultat de la recherche fait par MapPoint pour l'adresse de départ
        *               eResults        Le résultat de la recherche fait par MapPoint pour l'adresse de destination
        * 
        *********************************************************************************************************
        */
        public static void GetRoute(FindResults sResults, FindResults eResults)
        {
            SegmentSpecification[] routeSegment;
            routeSegment = new SegmentSpecification[2];

            routeSegment[0] = new SegmentSpecification();
            routeSegment[0].Waypoint = new Waypoint();
            routeSegment[0].Waypoint.Name = sResults.Results[0].FoundLocation.Entity.Name;
            routeSegment[0].Waypoint.Location = sResults.Results[0].FoundLocation;

            routeSegment[1] = new SegmentSpecification();
            routeSegment[1].Waypoint = new Waypoint();
            routeSegment[1].Waypoint.Name = eResults.Results[0].FoundLocation.Entity.Name;
            routeSegment[1].Waypoint.Location = eResults.Results[0].FoundLocation;


            RouteSpecification routeSpecs = new RouteSpecification();
            routeSpecs.DataSourceName = "MapPoint.NA";
            routeSpecs.Segments = routeSegment;
    
            RouteServiceSoap routeService = new RouteServiceSoap();
            routeService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
            routeService.PreAuthenticate = true;
            // routeService.Proxy = myProxy;

            Route route = new Route();
            route = routeService.CalculateRoute(routeSpecs);

            // Generate the Route Map
            MapSpecification mapSpec = new MapSpecification();
            mapSpec.Options = new MapOptions();
            mapSpec.Options.Format = new ImageFormat();

            //Set the map width and height 
            //according to the PictureBox
            mapSpec.Options.Format.Height = 260;
            mapSpec.Options.Format.Width = 240;

            //Set the Map Datasource
            mapSpec.DataSourceName = "MapPoint.NA";

            mapSpec.Route = route;

            try
            {
                // Get the map image
                RenderServiceSoap renderService = new RenderServiceSoap();
                renderService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
                MapImage tempImage = renderService.GetMap(mapSpec)[0];
                Bitmap myMap = new Bitmap(new MemoryStream(tempImage.MimeData.Bits, false), true);
                //pcMap.Image = myMap;
                FileStream stream = new FileStream("c:\\map\\map.bmp", FileMode.OpenOrCreate);
                myMap.Save(stream, System.Drawing.Imaging.ImageFormat.Bmp);
                stream.Close();
                System.Threading.Thread.Sleep(1000);
                //System.Diagnostics.Process.Start("c:\\map\\bmpcvt.exe","c:\\map\\map.bmp -convertintobestpalette -saveasmap,3 -exit");
                System.Diagnostics.Process.Start("c:\\map\\bmpcvt.exe", "c:\\map\\map.bmp -convertintobestpalette -saveasc:\\map\\toto.c,1 -exit");
                //stsMain.Text = "Done";//BmpCvt logo.bmp -convertintobestpalette -saveaslogo,1 -exit
            }
            catch (Exception e)
            {
                //MessageBox.Show(e.ToString());
            }
        }

        /*
        *********************************************************************************************************
        *                                              GetColis()
        *
        * Description : Cette fonction effectue le trajet de génère la carte en bitmap
        *
        * Argument(s) : OrigLat         Coordonnée GPS Latitude du point d'origine
        *               OrigLong        Coordonnée GPS Longitude du point d'origine
        *               DestLat         Coordonnée GPS Latitude du point de destination
        *               DestLong        Coordonnée GPS Longitude du point de destination
        * 
        *********************************************************************************************************
        */
        public static void GetRouteFromGps(double OrigLat, double OrigLong, double DestLat, double DestLong)
        {
            Location LocDepart = new Location();
            LocDepart.LatLong = new LatLong();
            LocDepart.LatLong.Latitude = OrigLat;
            LocDepart.LatLong.Longitude = OrigLong;

            Location LocArrivee = new Location();
            LocArrivee.LatLong = new LatLong();
            LocArrivee.LatLong.Latitude = DestLat;
            LocArrivee.LatLong.Longitude = DestLong;

            SegmentSpecification[] routeSegment;
            routeSegment = new SegmentSpecification[2];

            routeSegment[0] = new SegmentSpecification();
            routeSegment[0].Waypoint = new Waypoint();
            routeSegment[0].Waypoint.Name = "Depart";
            routeSegment[0].Waypoint.Location = LocDepart;

            routeSegment[1] = new SegmentSpecification();
            routeSegment[1].Waypoint = new Waypoint();
            routeSegment[1].Waypoint.Name = "Arrivee";
            routeSegment[1].Waypoint.Location = LocArrivee;


            RouteSpecification routeSpecs = new RouteSpecification();
            routeSpecs.DataSourceName = "MapPoint.NA";
            routeSpecs.Segments = routeSegment;
     
            RouteServiceSoap routeService = new RouteServiceSoap();
            routeService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
            routeService.PreAuthenticate = true;
            // routeService.Proxy = myProxy;

            Route route = new Route();
            route = routeService.CalculateRoute(routeSpecs);

            // Generate the Route Map
            MapSpecification mapSpec = new MapSpecification();
            mapSpec.Options = new MapOptions();
            mapSpec.Options.Format = new ImageFormat();

            //Set the map width and height 
            //according to the PictureBox
            mapSpec.Options.Format.Height = 260;
            mapSpec.Options.Format.Width = 240;

            //Set the Map Datasource
            mapSpec.DataSourceName = "MapPoint.NA"; ;

            mapSpec.Route = route;

            try
            {
                // Get the map image
                RenderServiceSoap renderService = new RenderServiceSoap();
                renderService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
                MapImage tempImage = renderService.GetMap(mapSpec)[0];

                Bitmap myMap = new Bitmap(new MemoryStream(tempImage.MimeData.Bits, false), true);

                FileStream stream = new FileStream("c:\\map\\map.bmp", FileMode.OpenOrCreate);
                myMap.Save(stream, System.Drawing.Imaging.ImageFormat.Bmp);
                stream.Close();

                System.Threading.Thread.Sleep(1000);
                //System.Diagnostics.Process.Start("c:\\map\\bmpcvt.exe","c:\\map\\map.bmp -convertintobestpalette -saveasmap,3 -exit");
                System.Diagnostics.Process.Start("c:\\map\\bmpcvt.exe", "c:\\map\\map.bmp -convertintobestpalette -saveasc:\\map\\toto.c,1 -exit");
                //stsMain.Text = "Done";//BmpCvt logo.bmp -convertintobestpalette -saveaslogo,1 -exit
            }
            catch (Exception e)
            {
                //MessageBox.Show(e.ToString());
            }
        }       
    }
}
