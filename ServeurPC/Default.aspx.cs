/*
*********************************************************************************************************
* Fichier : Default.aspx.cs
* Par     : Marc-Étienne Lebeau, Julien Beaumier-Ethier, Richard Labonté, Francis Robichaud, Julien Marchand
* Date    : 2007/04/13
*********************************************************************************************************
*/
using MySql.Data.MySqlClient;
using net.mappoint.staging;
using System;
using System.Collections;
using System.Data;
using System.Drawing;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;

public partial class _Default : System.Web.UI.Page
{
    #region VARIABLES GLOBALES
    // Chargement du "connection string" définit dans le fichier web.config
    private string str_ConnString = ConfigurationSettings.AppSettings["ConnectionString"];

    private MySqlConnection m_SqlConnection;
    static bool onMsgDiv;
    static bool onListeDiv;
    static bool onCarteDiv;
    bool JourneeFlag = false;

    double CentralCoordLat = -71.9287836211068;
    double CentralCoordLong = 45.3821264306392;

    const char COMMAND_DELIMITER = ';';
    const byte COMMAND_GETMSGS = 53;
    const byte COMMAND_MSGFROMPDA = 54;
    const byte COMMAND_MSGTOPDA = 55;

    //static TestSemaphore.Semaphore usingSocket = new TestSemaphore.Semaphore(2);
    #endregion

    #region PAGE LOAD/UNLOAD
    /*
    *********************************************************************************************************
    *                                              Page_Load()
    *
    * Description : Cette fonction est appelée lors de chaque chargement/rafraîchissement de la page web
    *
    * Notes		  : La fonction permet de gérer l'affichage du contenu de l'interface web. Elle effectue
    *               aussi un chargement du contenu des menus déroulant des camions lors du premier chargement
    *               de la page web via une requête SQL 
    *********************************************************************************************************
    */
    protected void Page_Load(object sender, EventArgs e)
    {
        divMsg.Visible = false;
        divCamion.Visible = false;
        divListe.Visible = false;
        divCarte.Visible = false;
        lblError.Visible = false;
        lblErrorCam.Visible = false;
       
        Timer2_Tick(null, null);

        // Ce code est roulé lors du premier chargement de la page web
        if (!IsPostBack)
        {
            try
            {
                string str_Sql = "";
                MySqlConnection MyConnection = GetConnection();
                MySqlCommand MyCommand = null;
                MySqlDataReader MyReader = null;

                str_Sql = "SELECT cam_nom FROM camion";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                // Emplissage des menus déroulant des camions
                while (MyReader.Read())
                {
                    dropCamion.Items.Add(MyReader[0].ToString());
                    dropRetirer.Items.Add(MyReader[0].ToString());
                    ddlCamionCarte.Items.Add(MyReader[0].ToString());
                }

                MyReader.Close();
            }
            catch (MySqlException myEx)
            {
            }
            //onMsgDiv = false;
            //onCarteDiv = false;
            cmdFinJournee.Enabled = false;
        }
    }

    /*
    *********************************************************************************************************
    *                                              OnUnload()
    *
    * Description : Cette fonction est appelée lors de chaque "unload" de la page web
    *
    * Notes		  : La fonction effectue une fermeture de la connexion avec la base de données.
    *********************************************************************************************************
    */
    protected override void OnUnload(EventArgs e)
    {
        CloseConnection();
        base.OnUnload(e);
    }
    #endregion

    #region GESTION TABS
    /*
    *********************************************************************************************************
    *                                              cmd_Ajout_Click()
    *
    * Description : Cette fonction est appelée lors d'un clique sur le tab "Ajouter un colis"
    *
    * Notes		  : La fonction effectue la gestion de l'affichage du contenu du tab "Ajouter un colis"
    *********************************************************************************************************
    */
    protected void cmd_Ajout_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.DarkOrange;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.Yellow;
        cmd_Camion.BackColor = Color.Yellow;
        divAjout.Visible = true;
        divMsg.Visible = false;
        divListe.Visible = false;
        divCarte.Visible = false;
        divCamion.Visible = false;

        onMsgDiv = false;
        onListeDiv = false;
        onCarteDiv = false;
    }

    /*
    *********************************************************************************************************
    *                                              cmd_Carte_Click()
    *
    * Description : Cette fonction est appelée lors d'un clique sur le tab "Carte de la ville"
    *
    * Notes		  : La fonction effectue la gestion de l'affichage du contenu du tab "Carte de la ville"
    *********************************************************************************************************
    */
    protected void cmd_Carte_Click(object sender, EventArgs e)
    {
        Timer3_Tick(null, null);
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.DarkOrange;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.Yellow;
        cmd_Camion.BackColor = Color.Yellow;
        divAjout.Visible = false;
        divCarte.Visible = true;
        divCamion.Visible = false;
        divMsg.Visible = false;
        divListe.Visible = false;
        
        onMsgDiv = false;
        onListeDiv = false;
        onCarteDiv = true;
    }

    /*
    *********************************************************************************************************
    *                                              cmd_ListeColis_Click()
    *
    * Description : Cette fonction est appelée lors d'un clique sur le tab "Liste des colis"
    *
    * Notes		  : La fonction effectue la gestion de l'affichage du contenu du tab "Liste des colis"
    *********************************************************************************************************
    */
    protected void cmd_ListeColis_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.DarkOrange;
        cmd_Msg.BackColor = Color.Yellow;
        cmd_Camion.BackColor = Color.Yellow;
        divAjout.Visible = false;
        divListe.Visible = true;
        divCamion.Visible = false;
        divMsg.Visible = false;
        divCarte.Visible = false;

        onMsgDiv = false;
        onListeDiv = true;
        onCarteDiv = false;
    }

    /*
    *********************************************************************************************************
    *                                              cmd_Msg_Click()
    *
    * Description : Cette fonction est appelée lors d'un clique sur le tab "Messages"
    *
    * Notes		  : La fonction effectue la gestion de l'affichage du contenu du tab "Messages"
    *********************************************************************************************************
    */
    protected void cmd_Msg_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.DarkOrange;
        cmd_Camion.BackColor = Color.Yellow;
        divAjout.Visible = false;
        divMsg.Visible = true;
        divCamion.Visible = false;
        divListe.Visible = false;
        divCarte.Visible = false;

        Timer1_Tick(null, null);

        onMsgDiv = true;
        onListeDiv = false;
        onCarteDiv = false;
    }

    /*
    *********************************************************************************************************
    *                                              cmd_Camion_Click()
    *
    * Description : Cette fonction est appelée lors d'un clique sur le tab "Gestion des camions"
    *
    * Notes		  : La fonction effectue la gestion de l'affichage du contenu du tab "Gestion des camions"
    *********************************************************************************************************
    */
    protected void cmd_Camion_Click(object sender, EventArgs e)
    {

        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.Yellow;
        cmd_Camion.BackColor = Color.DarkOrange;
        divAjout.Visible = false;
        divMsg.Visible = false;
        divCamion.Visible = true;
        divListe.Visible = false;
        divCarte.Visible = false;

        onMsgDiv = false;
        onCarteDiv = false;
        onListeDiv = false;
    }
    #endregion

    #region TAB AJOUT COLIS
    /*
    *********************************************************************************************************
    *                                              cmdValiderAjout_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Ajouter ce colis"
    *
    * Notes		  : La fonction vérifie si le numéro de colis à ajouter n'est présent dans la BD.
    *               Le cas échéant, elle effectue une requête SQL "INSERT INTO" permettant d'ajouter les 
    *               champs entrés par l'utilisateur dans la base de données "colis". 
    *********************************************************************************************************
    */
    protected void cmdValiderAjout_Click(object sender, EventArgs e)
    {
        try
        {
            string str_Sql = "";
            string str_PlageDebutCli = "";
            string str_PlageFinCli = "";
            string str_PlageDebutDest = "";
            string str_PlageFinDest = "";
            string str_EtatColis = "0";
            string str_AddressCli = "";
            string str_AddressDest = "";
            string[] str_LongLatCli = new string[2];
            string[] str_LongLatDest = new string[2];
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            // Verification de la presence d'un numero de colis dans la BD
            str_Sql = "SELECT * FROM colis WHERE col_noident='" + txt_Ident.Text + "'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            // Si le colis des déjà présent dans la BD, afficher label d'erreur
            if (MyReader.Read())
            {
                lblError.Text = "Le colis existe déjà!";
                lblError.Visible = true;
                MyReader.Close();
            }
            else
            {
                MyReader.Close();
                if (rdb_Etat3.Checked == true)
                {
                    lblError.Text = "État initial invalide";
                    lblError.Visible = true;
                }
                else if (rdb_Etat4.Checked == true)
                {
                    lblError.Text = "État initial invalide";
                    lblError.Visible = true;
                }
                else
                {
                    if (rdb_Etat1.Checked == true)
                    {
                        str_EtatColis = "0";
                    }
                    else if (rdb_Etat2.Checked == true)
                    {
                        str_EtatColis = "1";
                    }
                    else if (rdb_Etat3.Checked == true)
                    {
                        str_EtatColis = "2";
                    }
                    else if (rdb_Etat4.Checked == true)
                    {
                        str_EtatColis = "3";
                    }

                    str_PlageDebutCli = txt_PlageClient1.Text + ":00";
                    str_PlageFinCli = txt_PlageClient2.Text + ":00";
                    str_PlageDebutDest = txt_PlageDest1.Text + ":00";
                    str_PlageFinDest = txt_PlageDest2.Text + ":00";

                    // Génération des coordonnées GPS
                    str_AddressCli = txt_AdresseClient.Text + ";" + txt_VilleClient.Text + ";";
                    str_AddressCli += "QC;" + txt_CodePostalClient.Text + ";CA";
                    str_LongLatCli = GetGPSFromAdress(str_AddressCli);

                    str_AddressDest = txt_AdresseDest.Text + ";" + txt_VilleDest.Text + ";";
                    str_AddressDest += "QC;" + txt_CodePostalDest.Text + ";CA";
                    str_LongLatDest = GetGPSFromAdress(str_AddressDest);

                    string str_CamionName = "";
                    string str_Ordre = "-1";
                    if (str_EtatColis == "0" && JourneeFlag)
                    {
                        AssignClosestCamion(str_LongLatDest, ref str_CamionName, ref str_Ordre);
                    }

                    str_Sql = "INSERT INTO colis (col_noident, col_nomcli, col_adrcli, col_villecli, col_cpcli, col_hrdebutcli, col_hrfincli, ";
                    str_Sql += "col_remarquecli, col_etat, col_nomdest, col_adrdest, col_villedest, col_cpdest, col_hrdebutdest, col_hrfindest, ";
                    str_Sql += "col_remarquedest, cam_nom, col_gpslongcli, col_gpslatcli, col_gpslongdest, col_gpslatdest, col_ordre) VALUES ('" + txt_Ident.Text + "', '" + txt_NomClient.Text + "', '" + txt_AdresseClient.Text;
                    str_Sql += "', '" + txt_VilleClient.Text + "', '" + txt_CodePostalClient.Text + "', '" + str_PlageDebutCli + "', '" + str_PlageFinCli + "', '";
                    str_Sql += txt_RemarquesClient1.Text + "', '" + str_EtatColis + "', '" + txt_NomDest.Text + "', '";
                    str_Sql += txt_AdresseDest.Text + "', '" + txt_VilleDest.Text + "', '" + txt_CodePostalDest.Text + "', '" + str_PlageDebutDest + "', '" + str_PlageFinDest;
                    str_Sql += "', '" + txt_RemarquesDest1.Text + "', '" + str_CamionName + "', '" + str_LongLatCli[0] + "', '" + str_LongLatCli[1];
                    str_Sql += "', '" + str_LongLatDest[1] + "', '" + str_LongLatDest[1] + "', '" + str_Ordre + "')";

                    MyCommand = new MySqlCommand(str_Sql, MyConnection);
                    MyCommand.ExecuteNonQuery();
                }
            }
            MyReader.Close();
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              cmdValiderRecherche_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Rechercher 
    *               ce colis".
    *
    * Notes		  : La fonction permet de rechercher un colis dans la BD en effectuant une requête SQL
    *               "SELECT" dans la table "colis".
    *********************************************************************************************************
    */
    protected void cmdValiderRecherche_Click(object sender, EventArgs e)
    {
        try
        {
            string str_Sql = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            str_Sql = "SELECT * FROM colis WHERE col_noident='" + txt_Ident.Text + "'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                txt_Ident.Text = MyReader[1].ToString();
                txt_NomClient.Text = MyReader[2].ToString();
                txt_AdresseClient.Text = MyReader[3].ToString();
                txt_VilleClient.Text = MyReader[4].ToString();
                txt_CodePostalClient.Text = MyReader[5].ToString();
                txt_PlageClient1.Text = MyReader[6].ToString().Substring(0, 5);
                txt_PlageClient2.Text = MyReader[7].ToString().Substring(0, 5);
                txt_RemarquesClient1.Text = MyReader[8].ToString();

                // Gestion des "radio buttons"
                if (MyReader[9].ToString() == "0")
                {
                    rdb_Etat1.Checked = true;
                    rdb_Etat2.Checked = false;
                    rdb_Etat3.Checked = false;
                    rdb_Etat4.Checked = false;
                }
                else if (MyReader[9].ToString() == "1")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = true;
                    rdb_Etat3.Checked = false;
                    rdb_Etat4.Checked = false;
                }
                else if (MyReader[9].ToString() == "2")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = false;
                    rdb_Etat3.Checked = true;
                    rdb_Etat4.Checked = false;
                }
                else if (MyReader[9].ToString() == "3")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = false;
                    rdb_Etat3.Checked = false;
                    rdb_Etat4.Checked = true;
                }
                
                txt_NomDest.Text = MyReader[10].ToString();
                txt_AdresseDest.Text = MyReader[11].ToString();
                txt_VilleDest.Text = MyReader[12].ToString();
                txt_CodePostalDest.Text = MyReader[13].ToString();
                txt_PlageDest1.Text = MyReader[14].ToString().Substring(0, 5);
                txt_PlageDest2.Text = MyReader[15].ToString().Substring(0, 5);
                txt_RemarquesDest1.Text = MyReader[16].ToString();
            }
            MyReader.Close();
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              cmdValiderModif_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Modifier ce colis"
    *
    * Notes		  : La fonction effectue une requête SQL "UPDATE" permettant d'modifier les 
    *               champs de l'interface web entrés par l'utilisateur dans la base de données "colis". 
    *********************************************************************************************************
    */
    protected void cmdValiderModif_Click(object sender, EventArgs e)
    {
        try
        {
            string str_Sql = "";
            string str_PlageDebutCli = "";
            string str_PlageFinCli = "";
            string str_PlageDebutDest = "";
            string str_PlageFinDest = "";
            string str_EtatColis = "0";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;

            if (rdb_Etat1.Checked == true)
            {
                str_EtatColis = "0";
            }
            else if (rdb_Etat2.Checked == true)
            {
                str_EtatColis = "1";
            }
            else if (rdb_Etat3.Checked == true)
            {
                str_EtatColis = "2";
            }
            else if (rdb_Etat4.Checked == true)
            {
                str_EtatColis = "3";
            }

            str_PlageDebutCli = txt_PlageClient1.Text + ":00";
            str_PlageFinCli = txt_PlageClient2.Text + ":00";
            str_PlageDebutDest = txt_PlageDest1.Text + ":00";
            str_PlageFinDest = txt_PlageDest2.Text + ":00";

            str_Sql = "UPDATE colis SET col_noident = '" + txt_Ident.Text + "', col_nomcli = '" + txt_NomClient.Text;
            str_Sql += "', col_adrcli = '" + txt_AdresseClient.Text + "', col_villecli = '" + txt_VilleClient.Text + "', col_cpcli = '" + txt_CodePostalClient.Text + "', col_hrdebutcli = '";
            str_Sql += str_PlageDebutCli + "', col_hrfincli = '" + str_PlageFinCli + "', col_remarquecli = '" + txt_RemarquesClient1.Text;
            str_Sql += "', col_etat = '" + str_EtatColis + "', col_nomdest = '" + txt_NomDest.Text + "', col_adrdest = '";
            str_Sql += txt_AdresseDest.Text + "', col_villedest = '" + txt_VilleDest.Text + "', col_cpdest = '" + txt_CodePostalDest.Text + "', col_hrdebutdest = '" + str_PlageDebutDest;
            str_Sql += "', col_hrfindest = '" + str_PlageFinDest + "', col_remarquedest ='" + txt_RemarquesDest1.Text;
            str_Sql += "' WHERE col_noident = '" + txt_Ident.Text + "'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyCommand.ExecuteNonQuery();
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              cmdEffacerChamps_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Effacer les champs"
    *
    * Notes		  : La fonction efface tous les champs de l'interface web.
    *********************************************************************************************************
    */
    protected void cmdEffacerChamps_Click(object sender, EventArgs e)
    {
        txt_Ident.Text = "";
        txt_NomClient.Text = "";
        txt_NomDest.Text = "";
        txt_AdresseClient.Text = "";        
        txt_VilleClient.Text = "";
        txt_CodePostalClient.Text = "";
        txt_AdresseDest.Text = "";
        txt_VilleDest.Text = "";
        txt_CodePostalDest.Text = "";
        txt_PlageClient1.Text = "";
        txt_PlageClient2.Text = "";
        txt_PlageDest1.Text = "";        
        txt_PlageDest2.Text = "";
        txt_RemarquesClient1.Text = "";
        txt_RemarquesDest1.Text = "";
        rdb_Etat1.Checked = false;
        rdb_Etat2.Checked = false;
        rdb_Etat3.Checked = false;
        rdb_Etat4.Checked = false;
    }
#endregion

    #region TAB CARTE

    /*
   *********************************************************************************************************
   *                                              cmdGetCarte_Click()
   *
   * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Obtenir la carte"
   *               du tab "Carte de la ville"
   *
   * Notes		  : La fonction effectue une requête SQL "DELETE FROM" permettant de supprimer une entrée
   *               de la table "camion" de la BD. Elle met ensuite à jour les menus déroulant.
   *********************************************************************************************************
   */
    protected void cmdGetCarte_Click(object sender, EventArgs e)
    {

    }

    /*
    *********************************************************************************************************
    *                                              Timer3_Tick()
    *
    * Description : Cette fonction rafraîchit la carte illustrant le trajet entre la position actuelle du
    *               du camion et sa destination 
    *
    * Notes		  : La fonction est appelée par un module AJAX
    *********************************************************************************************************
    */
    protected void Timer3_Tick(object sender, EventArgs e)
    {
        if (onCarteDiv)
        {
            divCarte.Visible = true;
        }
        imgCarte.ImageUrl = "C:\\map\\map0.jpg";
    }

    #endregion

    #region TAB MESSAGE
    /*
    *********************************************************************************************************
    *                                              cmdEnvoyerMsg_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Envoyer" du tab
    *               Messages
    *
    * Notes		  : La fonction ouvre un socket TCP, stocke le contenu du message à envoyer dans un buffer
    *               et envoie le contenu du buffer sur le socket 
    *********************************************************************************************************
    */
    protected void cmdEnvoyerMsg_Click(object sender, EventArgs e)
    {
        int hour = DateTime.Now.Hour;
        int min = DateTime.Now.Minute;
        string minStr;

        if (min < 10)
            minStr = "0" + min.ToString();
        else
            minStr = min.ToString();
        
        //Envoi du message par socket TCP
        Socket sendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        //IPAddress remoteIP = (System.Net.IPAddress)Dns.GetHostAddresses("skaber.mine.nu").GetValue(0);
        IPAddress remoteIP = IPAddress.Parse("127.0.0.1");
        
        const int remotePort = 2160;
        IPEndPoint connectTo = new IPEndPoint(remoteIP, remotePort); ;
        
        //usingSocket.Wait();
        sendSocket.Connect(connectTo);
        byte[] buf = new byte[200];
        buf[0] = (byte) COMMAND_MSGTOPDA;

        for (int i = 0; i < TextEnvoiMsg.Text.Length; i++)
        {
            buf[i + 1] = Encoding.ASCII.GetBytes(TextEnvoiMsg.Text)[i];
        }
        buf[TextEnvoiMsg.Text.Length + 1] = (byte) COMMAND_DELIMITER;
        int bufferUsed = buf.Length;
        sendSocket.Send(buf);
        sendSocket.Close();
        //usingSocket.Release();

        TextEnvoiMsg.Text = "";
        divMsg.Visible = true;
    }

    /*
    *********************************************************************************************************
    *                                              cmdResetMsgRecus_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Effacer" du tab
    *               Messages
    *
    * Notes		  : La fonction efface le contenu du label affichant les messages reçus
    *********************************************************************************************************
    */
    protected void cmdResetMsgRecus_Click(object sender, EventArgs e)
    {
        divMsg.Visible = true;
        LabelMsgRecus.Text = "";
    }

    /*
    *********************************************************************************************************
    *                                              Timer1_Tick()
    *
    * Description : Cette fonction ouvre un socket de communication afin de permettre l'envoie et la
    *               réception de messages avec un PDA 
    *
    * Notes		  : La fonction est appelée par un module AJAX
    *********************************************************************************************************
    */
    protected void Timer1_Tick(object sender, EventArgs e)
    {
        byte[] bufMsg = new byte[1000];
        //string[] receivedMsg = new string[5];
        int bufMsgLength;
        char[] delimiter = COMMAND_DELIMITER.ToString().ToCharArray();

        try
        {
            Socket sendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPAddress remoteIP = IPAddress.Parse("127.0.0.1");
            const int remotePort = 2160;
            IPEndPoint connectTo = new IPEndPoint(remoteIP, remotePort); ;

            //usingSocket.Wait();
            sendSocket.Connect(connectTo);
            byte[] bufSignal = new byte[7];
            bufSignal[0] = (byte)COMMAND_GETMSGS;
            bufSignal[1] = (byte)COMMAND_DELIMITER;
            sendSocket.Send(bufSignal);
            //usingSocket.Release();

            //string myCurrentStr = "";

            //while (bufMsgLength > 0) {

            bufMsgLength = sendSocket.Receive(bufMsg);

            //usingSocket.Wait();
            if (bufMsgLength > 0)
            {
                string[] strReceivedData = Encoding.ASCII.GetString(bufMsg).Split(delimiter, 10);

                for (int j = 0; j < strReceivedData.GetLength(0); j++)
                {
                    if (strReceivedData[j][0] != '\0')
                    {
                        LabelMsgRecus.Text = strReceivedData[j].Substring(2, strReceivedData[j].Length - 2) + "\n" + LabelMsgRecus.Text;
                    }
                }

            }

            //    if (bufMsgLength > 0)
            //        myCurrentStr += System.Text.Encoding.ASCII.GetString(bufMsg, 0, bufMsgLength + 1);
            //}

            //int i = 0;

            //while (myCurrentStr != "") {
            //    if (myCurrentStr.IndexOf(';') != -1) {
            //        receivedMsg[i] = myCurrentStr.Substring(0, myCurrentStr.IndexOf(';'));

            //        try {
            //            myCurrentStr = myCurrentStr.Substring(myCurrentStr.IndexOf(';') + 2, ((int)myCurrentStr.Length - (myCurrentStr.IndexOf(';') + 2)));
            //        } catch {
            //            break;
            //        }
            //    } else {
            //        myCurrentStr = "";
            //    }
            //    i++;



            sendSocket.Close();
            //usingSocket.Release();

            if (onMsgDiv)
            {
                divMsg.Visible = true;
                TextEnvoiMsg.Focus();
            }

        }
        catch (SocketException exp)
        {
            //receivedMsg[0] = "Could not connect to message server" + exp.Message;
        }
    }
    #endregion

    #region TAB LISTE COLIS
    /*
    *********************************************************************************************************
    *                                              Timer2_Tick()
    *
    * Description : Cette fonction effectue une requête SQL à la base de données afin d'obtenir la liste
    *               des colis qu'il affiche ensuite dans le tab "Liste des colis" 
    *
    *********************************************************************************************************
    */
    protected void Timer2_Tick(object sender, EventArgs e)
    {
        string str_Sql = "";
        string str_Temp = "";
        MySqlConnection MyConnection = GetConnection();
        MySqlCommand MyCommand = null;
        MySqlDataReader MyReader = null;

        str_Sql = "SELECT col_noident, cam_nom, col_etat, col_nomdest, col_adrdest, col_villedest, ";
        str_Sql += "col_cpdest, col_hrdebutdest, col_hrfindest, col_remarquedest FROM colis";

        TBLListeColis.Rows.Clear();

        TableRow row = new TableRow();
        row.BackColor = Color.Black;

        // No ident.
        TableCell cell0 = new TableCell();
        Label label0 = new Label();
        label0.Text = "Ident";
        label0.ForeColor = Color.Yellow;
        cell0.Controls.Add(label0);
        row.Cells.Add(cell0);

        // Nom camion
        TableCell cell1 = new TableCell();
        Label label1 = new Label();
        label1.Text = "Camion";
        label1.ForeColor = Color.Yellow;
        cell1.Controls.Add(label1);
        row.Cells.Add(cell1);
        
        // Etat
        TableCell cell2 = new TableCell();
        Label label2 = new Label();
        label2.Text = "Etat";
        label2.ForeColor = Color.Yellow;
        cell2.Controls.Add(label2);
        row.Cells.Add(cell2);

        // Nom dest
        TableCell cell3 = new TableCell();
        Label label3 = new Label();
        label3.Text = "Destinataire";
        label3.ForeColor = Color.Yellow;
        cell3.Controls.Add(label3);
        row.Cells.Add(cell3);

        // Addresse dest
        TableCell cell4 = new TableCell();
        Label label4 = new Label();
        label4.Text = "Addresse";
        label4.ForeColor = Color.Yellow;
        cell4.Controls.Add(label4);
        row.Cells.Add(cell4);
        
        // Ville dest
        TableCell cell5 = new TableCell();
        Label label5 = new Label();
        label5.Text = "Ville";
        label5.ForeColor = Color.Yellow;
        cell5.Controls.Add(label5);
        row.Cells.Add(cell5);

        // CP dest
        TableCell cell6 = new TableCell();
        Label label6 = new Label();
        label6.Text = "Code postal";
        label6.ForeColor = Color.Yellow;
        cell6.Controls.Add(label6);
        row.Cells.Add(cell6);

        // Heure debut
        TableCell cell7 = new TableCell();
        Label label7 = new Label();
        label7.Text = "Heure début";
        label7.ForeColor = Color.Yellow;
        cell7.Controls.Add(label7);
        row.Cells.Add(cell7);

        // Heure fin
        TableCell cell8 = new TableCell();
        Label label8 = new Label();
        label8.Text = "Heure fin";
        label8.ForeColor = Color.Yellow;
        cell8.Controls.Add(label8);
        row.Cells.Add(cell8);

        // Remarque
        TableCell cell9 = new TableCell();
        Label label9 = new Label();
        label9.Text = "Remarques";
        label9.ForeColor = Color.Yellow;
        cell9.Controls.Add(label9);
        row.Cells.Add(cell9);

        TBLListeColis.Rows.Add(row);

        MyCommand = new MySqlCommand(str_Sql, MyConnection);
        MyReader = MyCommand.ExecuteReader();

        while (MyReader.Read())
        {
            TableRow row1 = new TableRow();
            row1.BackColor = Color.Black;
            for (int i = 0; i < MyReader.FieldCount; i++)
            {
                if (i == 2)
                {
                    TableCell cell = new TableCell();
                    Label label = new Label();
                    str_Temp = MyReader[i].ToString();
                    if (str_Temp == "0")
                    {
                        label.Text = "non cueilli";
                    }
                    else if (str_Temp == "1")
                    {
                        label.Text = "cueilli";
                    }
                    else if (str_Temp == "2")
                    {
                        label.Text = "en livraison";
                    }
                    else if (str_Temp == "3")
                    {
                        label.Text = "livré";
                    }
                    cell.Controls.Add(label);
                    row1.Cells.Add(cell);
                }
                else
                {
                    TableCell cell = new TableCell();
                    Label label = new Label();
                    label.Text = MyReader[i].ToString();
                    cell.Controls.Add(label);
                    row1.Cells.Add(cell);
                }
            }
            TBLListeColis.Rows.Add(row1);
        }
        MyReader.Close();

        // Close connection if this update comes from a tick
        if (sender != null)
        {
            MyConnection.Close();
        }

        if (onListeDiv)
        {
            divListe.Visible = true;
        }
    }
    #endregion

    #region TAB CAMION
    /*
    *********************************************************************************************************
    *                                              cmdAjouterCamion_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Ajouter ce camion"
    *               du tab Gestion des camions
    *
    * Notes		  : La fonction effectue une requête SQL "INSERT INTO" permettant d'ajouter ne nouvelle
    *               entrée à la table "camion" de la BD. Elle met ensuite à jour les menus déroulant.
    *********************************************************************************************************
    */
    protected void cmdAjouterCamion_Click(object sender, EventArgs e)
    {
        divCamion.Visible = true;

        try
        {
            string str_Sql = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            // Verification de la presence d'un nom de camion dans la BD
            str_Sql = "SELECT * FROM camion WHERE cam_nom='" + txt_AjoutCamion.Text + "'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            if (MyReader.Read())
            {
                lblErrorCam.Visible = true;
            }
            else
            {
                str_Sql = "INSERT INTO camion (cam_nom) VALUES ('" + txt_AjoutCamion.Text + "')";

                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyCommand.ExecuteNonQuery();

                dropCamion.Items.Add(txt_AjoutCamion.Text);
                dropRetirer.Items.Add(txt_AjoutCamion.Text);

                txt_AjoutCamion.Text = "";
            }
            MyReader.Close();
        }
        catch (MySqlException myEx)
        {
        }       
    }
   /*
   *********************************************************************************************************
   *                                              cmdRetirerCamion_Click()
   *
   * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Retirer ce camion"
   *               du tab Gestion des camions
   *
   * Notes		  : La fonction effectue une requête SQL "DELETE FROM" permettant de supprimer une entrée
   *               de la table "camion" de la BD. Elle met ensuite à jour les menus déroulant.
   *********************************************************************************************************
   */
    protected void cmdRetirerCamion_Click(object sender, EventArgs e)
    {
        divCamion.Visible = true;

        try
        {
            string str_Sql = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;

            str_Sql = "DELETE FROM camion WHERE cam_nom='" + dropRetirer.SelectedValue + "'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyCommand.ExecuteNonQuery();

            dropCamion.Items.Remove(dropRetirer.SelectedValue);
            dropRetirer.Items.Remove(dropRetirer.SelectedValue);
        }
        catch (MySqlException myEx)
        {
        }

    }

    /*
    *********************************************************************************************************
    *                                              cmdDeterminationItineraire_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Déterminer les 
    *              itinéraires" du tab Gestion des camions
    *
    * Notes		  : La fonction effectue une requête de tous les colis et leur lieu de livraison (ou 
    *               ceuillette) et les attribue entre tous les camions existant dans la BD.
    *********************************************************************************************************
    */
    protected void cmdDeterminationItineraire_Click(object sender, EventArgs e)
    {
        divCamion.Visible = true;
        JourneeFlag = true;
        ArrayList ResultList = new ArrayList();
        try
        {
            ArrayList ColisList = new ArrayList();
            GetColisForItt(ref ColisList, 1);
            int CamionQuantity = 0;
            GetCamionQuantity(ref CamionQuantity);
            ResultList = DetermineItinerary(ColisList, CamionQuantity);
            UpdateBDItt(ResultList);
            cmdFinJournee.Enabled = true;
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              cmdFinJournee_Click()
    *
    * Description : Cette fonction est appelée lorsque l'utilisateur clique sur le bouton "Fin de la journée" 
    *               du tab Gestion des camions
    *
    * Notes		  : La fonction prend tous les colis de la BD dont l'état est encore à "En livraison" et 
    *               les remet à l'état cueilli.
    *********************************************************************************************************
    */
    protected void cmdFinJournee_Click(object sender, EventArgs e)
    {
        divCamion.Visible = true;
        JourneeFlag = false;
        try
        {
            UpdateBDFinJournee();
        }
        catch (MySqlException myEx)
        {
        }
        cmdFinJournee.Enabled = false;
    }
    #endregion

    #region GESTION BD
    /*
    *********************************************************************************************************
    *                                              GetConnection()
    *
    * Description : Cette fonction établit une connexion avec la base de données.
    *
    * Retourne    : MySqlConnection        La connexion avec la base de données MySql
    *********************************************************************************************************
    */
    public MySqlConnection GetConnection()
    { 
        if (m_SqlConnection == null) {
            m_SqlConnection = new MySqlConnection();
        }
        if (m_SqlConnection.State == ConnectionState.Closed) {
            m_SqlConnection.ConnectionString = str_ConnString;
            m_SqlConnection.Open();
        }

        return m_SqlConnection;
    }

    /*
    *********************************************************************************************************
    *                                              CloseConnection()
    *
    * Description : Cette fonction ferme une connexion avec la base de données.
    *
    *********************************************************************************************************
    */
    private void CloseConnection() 
    {
        if (m_SqlConnection != null && m_SqlConnection.State == ConnectionState.Open) {
            m_SqlConnection.Close();
        }
    }
    #endregion

    #region MAPPOINT
    /*
    *********************************************************************************************************
    *                                              GetGPSFromAdress()
    *
    * Description : Cette fonction converti une adresse en coordonnées GPS
    *
    * Argument(s) : strOrigAddress  L'adresse de départ 
    * 
    * Note        : Le format du string à passer en argument à la fonction est celui-ci:
    *               "ADRESSE;VILLE;PROVINCE;CODE POSTAL;PAYS"  
    *               ex: strOrigAddress = "1408 RUE DE L'EGLISE;SAINT-LAURENT;QC;H4L2H3;CA";
    *********************************************************************************************************
    */
    public static string[] GetGPSFromAdress(string strOrigAddress)
    {
        FindServiceSoap findService = new FindServiceSoap();
        findService.Credentials = new System.Net.NetworkCredential("124624", "PDALE_projets5");
        FindSpecification findSpec = new FindSpecification();
        FindResults startResults = null;
        FindResults endResults = null;


        //Output the formatted address
        string[] strTemp = new String[5];
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
            }
        }

        string[] strLongLat = new string[2];
        strLongLat[0] = startResults.Results[0].FoundLocation.LatLong.Longitude.ToString();
        strLongLat[1] = startResults.Results[0].FoundLocation.LatLong.Latitude.ToString();

        return strLongLat;
    }
    #endregion

    #region GESTION ITTINERAIRE
    /*
   *********************************************************************************************************
   *                                              GetCamionQuantity()
   *
   * Description : Cette fonction fournit le nombre  de camion se retrouvant dans la BD.
   *
   * Argument    : CamionQuantity  int représentant le nombre de camion dans la BD
   *********************************************************************************************************
   */
    public void GetCamionQuantity(ref int CamionQuantity)
    {
        try
        {
            string str_Sql = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;
            CamionQuantity = 0;

            // Verification de la presence d'un numero de colis dans la BD
            str_Sql = "SELECT cam_numero FROM camion";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                CamionQuantity++;
            }
            MyReader.Close();
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
   *********************************************************************************************************
   *                                              GetCamionNameFromID()
   *
   * Description : Cette fonction fournit le nom d'un camion associé à son ID dans la BD.
   *
   * Argument    : CamionID    int représentant le ID du camion dans la BD
   *               CamionName  string représentant le nom du camion dans la BD
   *********************************************************************************************************
   */
    public void GetCamionNameFromID(int CamionID, ref string CamionName)
    {
        try
        {
            string str_Sql = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;
            int temp = CamionID+1;
            string str_CamionID = temp.ToString();

            // Verification de la presence d'un numero de colis dans la BD
            str_Sql = "SELECT cam_nom FROM camion WHERE cam_numero = '" + str_CamionID + "'"; ;

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                CamionName = MyReader[0].ToString();
            }
            MyReader.Close();
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              GetColisForItt()
    *
    * Description : Cette fonction selectionne tous les colis de la bd ayant comme état 0 ou 1 et insère leur
    *               id et coordonnées gps dans un ArrayList 
    *
    *
    * Argument    : arr_ColisToReturn  ArrayList contenant tous les colis correspondant
    *               NewCueiletteFlag   Flag qui détermine si on cherche les colis pour batir l'itineraire (1)
    *                                  ou si l'on est en mode d'ajout d'une cueilette dans la BD. (2)
    *********************************************************************************************************
    */
    public void GetColisForItt(ref ArrayList arr_ColisToReturn, int NewCueiletteFlag)
    {
        try
        {
            string str_Sql = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;
            arr_ColisToReturn = new ArrayList();

            str_Sql = "SELECT col_gpslongcli, col_gpslatcli, col_noident FROM colis WHERE col_etat='0'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                ArrayList arr_Colis = new ArrayList();
                arr_Colis.Add(MyReader[0].ToString());
                arr_Colis.Add(MyReader[1].ToString());
                arr_Colis.Add(MyReader[2].ToString());
                arr_ColisToReturn.Add(arr_Colis);
            }
            MyReader.Close();

            str_Sql = "SELECT col_gpslongdest, col_gpslatdest, col_noident FROM colis WHERE col_etat='" + NewCueiletteFlag + "'";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                ArrayList arr_Colis = new ArrayList();
                arr_Colis.Add(MyReader[0].ToString());
                arr_Colis.Add(MyReader[1].ToString());
                arr_Colis.Add(MyReader[2].ToString());
                arr_ColisToReturn.Add(arr_Colis);
            }
            MyReader.Close();
        }
        catch (MySqlException myEx)
        {
        }       
    }

    /*
    *********************************************************************************************************
    *                                              UpdateBDItt()
    *
    * Description : Cette fonction met à jour le camion affecté à un colis
    *
    * Argument(s) : arr_ColCam  ArrayList contenant les colis et leur camions 
    *
    *********************************************************************************************************
    */
    public void UpdateBDItt(ArrayList arr_ColCam)
    {
        try
        {
            string str_Sql = "";
            string str_Ordre = "";
            string str_CamionName = "";
            string str_Etat = "";
            MySqlConnection MyConnection = GetConnection();
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;
           

            for (int i = 0; i < arr_ColCam.Count; i++)
            {
                GetCamionNameFromID(Int32.Parse(((ArrayList)arr_ColCam[i])[1].ToString()), ref str_CamionName);
                str_Ordre = (i + 1).ToString();

                // Selection de l'etat du colis courant
                str_Sql = "SELECT col_etat FROM colis WHERE col_noident='" + ((ArrayList)arr_ColCam[i])[0] + "'";
                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyReader = MyCommand.ExecuteReader();

                while (MyReader.Read())
                {
                    str_Etat = MyReader[0].ToString();
                }
                MyReader.Close();

                // Si l'état du colis était non cueilli, son état reste le même
                if (str_Etat == "0")
                {
                    str_Sql = "UPDATE colis SET cam_nom = '" + str_CamionName + "', col_ordre = '" + str_Ordre + "' ";
                    str_Sql += "WHERE col_noident = '" + ((ArrayList)arr_ColCam[i])[0] + "'";
                    MyCommand = new MySqlCommand(str_Sql, MyConnection);
                    MyCommand.ExecuteNonQuery();
                }

                // Si l'état du colis était cueilli, son état devient "en livraison"
                else if (str_Etat == "1")
                {
                    str_Sql = "UPDATE colis SET cam_nom = '" + str_CamionName + "', col_ordre = '" + str_Ordre + "', ";
                    str_Sql += "col_etat = '2' WHERE col_noident = '" + ((ArrayList)arr_ColCam[i])[0] + "'";
                    MyCommand = new MySqlCommand(str_Sql, MyConnection);
                    MyCommand.ExecuteNonQuery();
                }
            }  
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              UpdateBDFinJournee()
    *
    * Description : Cette fonction met à jour le camion affecté à un colis.
    *
    *********************************************************************************************************
    */
    public void UpdateBDFinJournee()
    {
        try
        {
            string str_Sql = "";
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;
            MySqlConnection MyConnection = GetConnection();

            ArrayList arr_Colis = new ArrayList();

            str_Sql = "SELECT col_noident FROM colis";

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                arr_Colis.Add(MyReader[0].ToString());
            }
            MyReader.Close();

            for (int i = 0; i < arr_Colis.Count; i++)
            {
                str_Sql = "UPDATE colis SET cam_nom = '' WHERE col_noident = '" + arr_Colis[i].ToString() + "'";                
                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyCommand.ExecuteNonQuery();
            }

            arr_Colis.Clear();
            str_Sql = "SELECT col_noident FROM colis WHERE col_etat='2'";
            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                arr_Colis.Add(MyReader[0].ToString());
            }
            MyReader.Close();

            for (int i = 0; i < arr_Colis.Count; i++)
            {
                str_Sql = "UPDATE colis SET col_etat = '1' WHERE col_noident = '" + arr_Colis[i].ToString() + "'";
                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyCommand.ExecuteNonQuery();
            }
        }
        catch (MySqlException myEx)
        {
        }
    }

    /*
    *********************************************************************************************************
    *                                              DetermineItinerary()
    *
    * Description : Cette fonction determine litineraire de chaque camion au début de la journée.
    * 
    * Arguments   : CoordsList      Arraylist contenant les colis de la BD et leur position GPS
    *               TrucksQuantity  Le nombre de camions dans la BD
    *
    * Return      : Arraylist d'association entre le ID d'un colis et son camion
    *
    *********************************************************************************************************
    */
    private ArrayList DetermineItinerary(ArrayList CoordsList, int TrucksQuantity)
    {
        // initialisation des variables locales
        ArrayList ReturnList = new ArrayList();
        ArrayList OnePair = new ArrayList();     

        double[,] TrucksCurrentState = new double[TrucksQuantity, 3];
        for (int i = 0; i < TrucksQuantity; i++)
        {
            TrucksCurrentState[i, 0] = CentralCoordLong; // Position X actuel
            TrucksCurrentState[i, 1] = CentralCoordLat;  // Position Y actuel
            TrucksCurrentState[i, 2] = 0.0;              // Distance totale parcourue
        }

        int    count = 0;
        int    CurrentTruck     = 0;
        double CurrentCoordLong = 0.0;
        double CurrentCoordLat  = 0.0;
        int    ClosestPackage   = 0;
        int    nbrColis = CoordsList.Count;
        for (int j = 0; j < nbrColis; j++)
        {
            // Cerner le prochain camion qui se verra attribuer un colis
            // Ce sera le camion qui aura parcouru la plus petite distance jusqua date
            double SmallestTotalDistance = 0.0;
            double TotalDistance = 0.0;
            for (int k = 0; k < TrucksQuantity; k++)
            {
                TotalDistance = TrucksCurrentState[k, 2];
                if (k == 0 || TotalDistance < SmallestTotalDistance)
                {
                    SmallestTotalDistance = TotalDistance;
                    CurrentTruck = k;
                }
            }

            CurrentCoordLong = TrucksCurrentState[CurrentTruck, 0];
            CurrentCoordLat  = TrucksCurrentState[CurrentTruck, 1];

            // Trouver le colis le plus pres
            double DistanceToAdd = 0.0;
            ClosestPackage = FindClosestPackage(CoordsList, CurrentCoordLong, CurrentCoordLat, ref DistanceToAdd);

            // Mettre le camion a cette position et mettre a jour la distance parcourue
            TrucksCurrentState[CurrentTruck, 0] = Convert.ToDouble((((ArrayList)CoordsList[ClosestPackage])[0]));
            TrucksCurrentState[CurrentTruck, 1] = Convert.ToDouble((((ArrayList)CoordsList[ClosestPackage])[1]));
            TrucksCurrentState[CurrentTruck, 2] = TrucksCurrentState[CurrentTruck, 2] + DistanceToAdd;

            // Batir la liste dassociation colis-camion pour le retour
            OnePair = new ArrayList();
            OnePair.Add(((ArrayList)CoordsList[ClosestPackage])[2].ToString()); // Package ID
            OnePair.Add(CurrentTruck.ToString()); // Truck ID
            ReturnList.Add(OnePair);

            // Eliminer le colis retenu de la liste
            CoordsList.RemoveAt(ClosestPackage);
        }

        return ReturnList;
    }

    /*
    *********************************************************************************************************
    *                                              FindClosestPackage()
    *
    * Description : Cette fonction trouve le colis le plus près d'un camion
    * 
    * Arguments   : CoordsList        Arraylist contenant les colis de la BD et leur position GPS
    *               CurrentCoordLong  La longitude du camion en cours
    *               CurrentCoordLat   La latitude du camion en cours 
    *               ClosestDistance   La distance du colis le plus près
    *
    * Return      : L'indice de CoordsList correspondant au colis le plus près 
    *
    *********************************************************************************************************
    */
    private int FindClosestPackage(ArrayList CoordsList, double CurrentCoordLong, double CurrentCoordLat, ref double ClosestDistance)
    {
        // Initialisation des variables locales
        int ReturnClosest = 0;
        double Distance   = 0.0;

        // Algorithme conservant le colis le plus pres du camion en cours
        for (int i = 0; i < CoordsList.Count; i++)
        {
            Distance = System.Math.Abs(CurrentCoordLong - Convert.ToDouble((((ArrayList)CoordsList[i])[0]))) + System.Math.Abs(CurrentCoordLat - Convert.ToDouble(((ArrayList)CoordsList[i])[1]));
            if (i == 0 || Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ReturnClosest = i;
            }
        }

        return ReturnClosest;
    }

    /*
    *********************************************************************************************************
    *                                              AssignClosestCamion()
    *
    * Description : Cette fonction trouve le camion qui sera le plus près du nouveau colis
    * 
    * Arguments   : str_LongLat       Tableau de string avec la position du nouveau colis
    *               str_CamionName    Le nom du camion qui sera assigné au colis
    *               str_Ordre         L'ordre que prendra le nouveau colis
    * Return      : 
    *
    *********************************************************************************************************
    */
    private void AssignClosestCamion(string[] str_LongLat, ref string str_CamionName, ref string str_Ordre)
    {
        try
        {
            string str_Sql = "";
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;
            MySqlConnection MyConnection = GetConnection();

            ArrayList ColisList = new ArrayList();
            GetColisForItt(ref ColisList, 2);
            double Distance = 0;
            int ClosestPackage = FindClosestPackage(ColisList, double.Parse(str_LongLat[0]), double.Parse(str_LongLat[1]), ref Distance);

            str_Sql = "SELECT col_cam, col_ordre FROM colis WHERE col_noident = '" + ((ArrayList)ColisList[ClosestPackage])[2].ToString() + "'"; // PackageID

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                str_CamionName = MyReader[0].ToString();
                str_Ordre = MyReader[1].ToString();
            }
            MyReader.Close();
            str_Ordre = (Int32.Parse(str_Ordre) + 1).ToString();

            str_Sql = "SELECT col_noident FROM colis WHERE col_ordre >= '" + str_Ordre + "'";
            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            ArrayList Colis = new ArrayList();
            int NombreColisApres = 0;
            while (MyReader.Read())
            {
                Colis.Add(MyReader.ToString());
                NombreColisApres++;
            }
            MyReader.Close();

            for (int j = 0; j < NombreColisApres; j++)
            {
                str_Sql = "UPDATE colis SET col_ordre = '" + str_Ordre + j + 1 + "' WHERE col_noident = '" + Colis[j] + "'";
                MyCommand = new MySqlCommand(str_Sql, MyConnection);
                MyCommand.ExecuteNonQuery();
            }
        }
        catch (MySqlException myEx)
        {
        }
    }
#endregion

}
