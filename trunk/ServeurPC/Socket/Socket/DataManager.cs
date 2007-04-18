using Socket.net.mappoint.staging;
using MySql.Data.MySqlClient;
using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections;

namespace Socket.Data
{
public class GPSCoord {
    public double Latitude = 45.37796;
    public double Longitude = -71.9243;
}

public class DataManager
{
    public GPSCoord[] GpsData;
    private MemoryStream[] m_GPSMaps;
    public Hashtable TrucksMapping;

    private static MySqlConnection m_SqlConnection;
    private static string str_ConnString =
            "Server=69.16.250.95;" +
            "Database=pdale;" +
            "User ID=pdale;" +
            "Password=projets5;";

    // Open a sql connection with the database
    public DataManager()
    {
        m_SqlConnection = GetConnection();
        TrucksMapping = new Hashtable();
        string[] trucks = GetTruckList().Split(';');

        for (int i = 0; i < trucks.Length; i++ ) {
            if (trucks[i] != string.Empty) {
                TrucksMapping.Add(i, trucks[i]);
            }
        }
        GpsData = new GPSCoord[TrucksMapping.Count];
        GpsData[0] = new GPSCoord();
        
        m_GPSMaps = new MemoryStream[TrucksMapping.Count];

        Thread mapgenerator = new Thread(new ThreadStart(GenerateMaps));
        mapgenerator.Start();
    }

    #region COMBINAISON
        
    public MemoryStream GetCurrentMap() 
    {
        MemoryStream tempstream = m_GPSMaps[0];
        return tempstream;
    }

    #endregion

    #region SQL
    
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
    public static string GetColis(string str_ColIdent)
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
                str_DonneesColis += MyReader[8].ToString() + ";";   // État du colis
                str_DonneesColis += MyReader[2].ToString() + ";";   // Nom du client
                str_DonneesColis += MyReader[9].ToString() + ";";   // Nom du destinataire
                str_DonneesColis += MyReader[3].ToString() + ";";   // Adresse1 du client
                str_DonneesColis += MyReader[4].ToString() + ";";   // Adresse2 du client
                str_DonneesColis += MyReader[10].ToString() + ";";  // Adresse1 du destinataire
                str_DonneesColis += MyReader[11].ToString() + ";";  // Adresse2 du destinataire
                str_DonneesColis += MyReader[5].ToString() + ";";   // Plage Horaire de cueilette début
                str_DonneesColis += MyReader[6].ToString() + ";";   // Plage Horaire de cueilette fin
                str_DonneesColis += MyReader[12].ToString() + ";";  // Plage Horaire de livraison début
                str_DonneesColis += MyReader[13].ToString() + ";";  // Plage Horaire de livraison fin
                str_DonneesColis += MyReader[7].ToString() + "*";  // Remarque
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
    *                                              GetColisCoordonnes()
    *
    * Description : Cette fonction retourne les champs d'un colis séparés par un ";" dans un string
    *
    * Argument(s) : str_ColIdent    Le numéro d'identification du colis
    *
    * Return(s)   : string          La variable string contenant les champs d'un colis
    *********************************************************************************************************
    */
    public string GetColisCoord(string str_ColIdent)
    {
        try
        {
            string str_Sql = "";
            string str_DonneesColis = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            str_Sql = "SELECT col_etat, col_gpslatcli, col_gpslongcli, col_gpslongdest, col_gpslatdest FROM colis WHERE col_noident=" + str_ColIdent + " ORDER BY col_ordre ASC LIMIT 1";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            MyReader.Read();
            
            if (MyReader[0].ToString() == "0") {
                str_DonneesColis += MyReader[1].ToString() + ";";
                str_DonneesColis += MyReader[2].ToString() + ";";
            } else {
                str_DonneesColis += MyReader[3].ToString() + ";";
                str_DonneesColis += MyReader[4].ToString() + ";";
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
    *                                              GetCamionList()
    *
    * Description : Cette fonction retourne la liste des camions
    *
    * Return(s)   : string          La liste des camions séparés par un ";"
    *********************************************************************************************************
    */
    public static string GetTruckList()
    {
        try
        {
            string str_Sql = "";
            string str_CamionList = "";
            int compteur = 0;
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            str_Sql = "SELECT cam_nom FROM camion";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                str_CamionList += MyReader[0].ToString() + ";";
            }
            str_CamionList += "*";
            MyReader.Close();
            return str_CamionList;
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
    public static string GetNbrCamion()
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
    *                                              GetNomCamionFromIndex()
    *
    * Description : Cette fonction retourne le nom du camion correspondant à un index dans la BD
    *
    * Argument(s) : str_IdCamion    L'index du camion
    *
    * Return(s)   : string          Le nom du camion
    *********************************************************************************************************
    */
    public static string GetNomCamionFromIndex(string str_IdCamion)
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
    public static void SaveEtatColis(string str_IdColis, string str_EtatColis)
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
    *                                              GetColisList()
    *
    * Description : Cette fonction retourne la liste des colis et leur état pour un camion donné
    *
    * Argument(s) : str_NomCamion   Le nom du camion
    *
    * Return(s)   : string          La liste des colis et de leur état tous séparés par un ";"
    *********************************************************************************************************
    */
    public static string GetColisList(string str_NomCamion)
    {
        try
        {
            string str_Sql = "";
            string str_ColisList = "";
            int compteur = 0;
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            str_Sql = "SELECT col_noident, col_etat FROM colis WHERE cam_nom='" + str_NomCamion + "' ORDER BY col_ordre";

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

    #endregion

    #region MAPPOINT

    /*
     * Generate a map and keeps it in a buffer for each truck
     */
    public void GenerateMaps()
    {
        while (true) {
            try {
                m_SqlConnection = GetConnection();

                //for(int i=0; i<TrucksMapping.Count; i++) {
                string colis = GetColisList((string)TrucksMapping[0]);
                if (colis != null) {
                    string colisinfo = GetColisCoord(colis.Split(';')[0]);
                    if (colisinfo != null) {
                        double colislong = double.Parse(colisinfo.Replace(',', '.').Split(';')[0]);
                        double colislat = double.Parse(colisinfo.Replace(',', '.').Split(';')[1]);
                        MemoryStream tempstream = GetRouteFromGps(GpsData[0].Latitude, GpsData[0].Longitude, colislat, colislong);

                        if (m_GPSMaps[0] != null) {
                            m_GPSMaps[0].Close();
                        }
                        Bitmap bmp = new Bitmap(tempstream);
                        tempstream.Position = 0;
                        StreamWriter filestream = new StreamWriter("c:\\map0.jpg", false);
                        bmp.Save(filestream.BaseStream, System.Drawing.Imaging.ImageFormat.Jpeg);
                        filestream.Close();
                        m_GPSMaps[0] = tempstream;
                    } else {
                        Console.WriteLine("Les itinéraires des camions n'ont pas été fait: aucun colis n'est associé au camion courant, les maps ne seront donc pas générées");
                    }
                }
                //}
                Thread.Sleep(5000);
            } catch (MySqlException) {
                Console.WriteLine("GenerateMaps(): Could not connect to database");
            } catch (Exception exp) {
                Console.WriteLine("Coordonates are not valid: " +exp.Message);
            }
        }
    }

    /*
    *********************************************************************************************************
    *                                              GetDirectionsFromAdress()
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
        myOrigAddress.CountryRegion = "CA";

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
        myDestAddress.CountryRegion = "CA";

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
    *                                              GetRoute()
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
    *                                              GetRouteFromGps()
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
    public static MemoryStream GetRouteFromGps(double OrigLat, double OrigLong, double DestLat, double DestLong)
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
        mapSpec.Options.Format.Height = 248;
        mapSpec.Options.Format.Width = 240;

        //Set the Map Datasource
        mapSpec.DataSourceName = "MapPoint.NA"; ;

        mapSpec.Route = route;

        MemoryStream stream;

        // Get the map image
        RenderServiceSoap renderService = new RenderServiceSoap();
        renderService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
        MapImage tempImage = renderService.GetMap(mapSpec)[0];

        Bitmap myMap = new Bitmap(new MemoryStream(tempImage.MimeData.Bits, false), true);

        stream = new MemoryStream();
        myMap.Save(stream, System.Drawing.Imaging.ImageFormat.Jpeg);

        return stream;

        //System.Threading.Thread.Sleep(1000);
        //System.Diagnostics.Process.Start("c:\\map\\bmpcvt.exe","c:\\map\\map.bmp -convertintobestpalette -saveasmap,3 -exit");
        //System.Diagnostics.Process.Start("c:\\map\\bmpcvt.exe", "c:\\map\\map.bmp -convertintobestpalette -saveasc:\\map\\toto.c,1 -exit");
        //stsMain.Text = "Done";//BmpCvt logo.bmp -convertintobestpalette -saveaslogo,1 -exit

    }

    #endregion

}
}