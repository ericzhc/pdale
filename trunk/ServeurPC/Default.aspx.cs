/*
*********************************************************************************************************
* Fichier : Default.aspx.cs
* Par     : Marc-Étienne Lebeau, Julien Beaumier-Ethier, Richard Labonté, Francis Robichaud
* Date    : 2007/04/12
*********************************************************************************************************
*/
using System;
using System.Data;
using System.Drawing;
using System.Configuration;
using MySql.Data.MySqlClient;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Net;
using System.Net.Sockets;
using System.Threading;

public partial class _Default : System.Web.UI.Page 
{
    // Chargement du "connection string" définit dans le fichier web.config
    private string str_ConnString = ConfigurationSettings.AppSettings["ConnectionString"];

    private MySqlConnection m_SqlConnection;
    static bool onMsgDiv;
    static bool onListeDiv;

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
        lblError.Visible = false;
        lblErrorCam.Visible = false;
        divListe.Visible = false;

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

                int i = 0;

                // Emplissage des menus déroulant des camions
                while (MyReader.Read())
                {
                    dropAssign.Items.Add(MyReader[0].ToString());
                    dropCamion.Items.Add(MyReader[0].ToString());
                    dropRetirer.Items.Add(MyReader[0].ToString());
                }

                MyReader.Close();
            }
            catch (MySqlException myEx)
            {
            }
            onMsgDiv = false;
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

        onMsgDiv = false;
        onListeDiv = false;
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
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.DarkOrange;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.Yellow;
        cmd_Camion.BackColor = Color.Yellow;
        divAjout.Visible = false;

        onMsgDiv = false;
        onListeDiv = false;
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

        onMsgDiv = false;
        onListeDiv = true;
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

        onMsgDiv = true;
        onListeDiv = false;
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

        onMsgDiv = false;
    }

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
            }
            else
            {
                if (rdb_Etat3.Checked == true)
                {
                    lblError.Text = "État initial invalide";
                    lblError.Visible = true;
                }
                else if (rdb_Etat3.Checked == true)
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

                    str_Sql = "INSERT INTO colis (col_noident, col_nomcli, col_adrcli1, col_adrcli2, col_hrdebutcli, col_hrfincli, ";
                    str_Sql += "col_remarquecli, col_etat, col_nomdest, col_adrdest1, col_adrdest2, col_hrdebutdest, col_hrfindest, ";
                    str_Sql += "col_remarquedest, cam_nom) VALUES ('" + txt_Ident.Text + "', '" + txt_NomClient.Text + "', '" + txt_AdresseClient1.Text;
                    str_Sql += "', '" + txt_AdresseClient2.Text + "', '" + str_PlageDebutCli + "', '" + str_PlageFinCli + "', '";
                    str_Sql += txt_RemarquesClient1.Text + "', '" + str_EtatColis + "', '" + txt_NomDest.Text + "', '";
                    str_Sql += txt_AdresseDest1.Text + "', '" + txt_AdresseDest2.Text + "', '" + str_PlageDebutDest + "', '" + str_PlageFinDest;
                    str_Sql += "', '" + txt_RemarquesDest1.Text + "', '" + dropAssign.SelectedValue.ToString() + "')";

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
                txt_AdresseClient1.Text = MyReader[3].ToString();
                txt_AdresseClient2.Text = MyReader[4].ToString();
                txt_PlageClient1.Text = MyReader[5].ToString().Substring(0, 5);
                txt_PlageClient2.Text = MyReader[6].ToString().Substring(0, 5);
                txt_RemarquesClient1.Text = MyReader[7].ToString();

                // Gestion des "radio buttons"
                if (MyReader[8].ToString() == "0")
                {
                    rdb_Etat1.Checked = true;
                    rdb_Etat2.Checked = false;
                    rdb_Etat3.Checked = false;
                    rdb_Etat4.Checked = false;
                }
                else if (MyReader[8].ToString() == "1")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = true;
                    rdb_Etat3.Checked = false;
                    rdb_Etat4.Checked = false;
                }
                else if (MyReader[8].ToString() == "2")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = false;
                    rdb_Etat3.Checked = true;
                    rdb_Etat4.Checked = false;
                }
                else if (MyReader[8].ToString() == "3")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = false;
                    rdb_Etat3.Checked = false;
                    rdb_Etat4.Checked = true;
                }
                
                txt_NomDest.Text = MyReader[9].ToString();
                txt_AdresseDest1.Text = MyReader[10].ToString();
                txt_AdresseDest2.Text = MyReader[11].ToString();
                txt_PlageDest1.Text = MyReader[12].ToString().Substring(0, 5);
                txt_PlageDest2.Text = MyReader[13].ToString().Substring(0, 5);
                txt_RemarquesDest1.Text = MyReader[14].ToString();
                dropAssign.SelectedValue = MyReader[15].ToString();
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
            str_Sql += "', col_adrcli1 = '" + txt_AdresseClient1.Text + "', col_adrcli2 = '" + txt_AdresseClient2.Text + "', col_hrdebutcli = '";
            str_Sql += str_PlageDebutCli + "', col_hrfincli = '" + str_PlageFinCli + "', col_remarquecli = '" + txt_RemarquesClient1.Text;
            str_Sql += "', col_etat = '" + str_EtatColis + "', col_nomdest = '" + txt_NomDest.Text + "', col_adrdest1 = '";
            str_Sql += txt_AdresseDest1.Text + "', col_adrdest2 = '" + txt_AdresseDest2.Text + "', col_hrdebutdest = '" + str_PlageDebutDest;
            str_Sql += "', col_hrfindest = '" + str_PlageFinDest + "', col_remarquedest ='" + txt_RemarquesDest1.Text;
            str_Sql += "', cam_nom = '" + dropAssign.SelectedValue.ToString() + "' WHERE col_noident = '" + txt_Ident.Text + "'";

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
        txt_AdresseClient1.Text = "";        
        txt_AdresseClient2.Text = "";
        txt_AdresseDest1.Text = "";
        txt_AdresseDest2.Text = "";
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
        dropAssign.SelectedIndex = 0;
    }

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
        IPAddress remoteIP = IPAddress.Parse("127.0.0.1");
        const int remotePort = 216999;
        IPEndPoint connectTo = new IPEndPoint(remoteIP, remotePort); ;
        
        sendSocket.Connect(connectTo);
        byte[] buf = new byte[200];
        buf = System.Text.Encoding.ASCII.GetBytes(TextEnvoiMsg.Text + "\0");
        int bufferUsed = buf.Length;
        sendSocket.Send(buf);
        sendSocket.Close();

        LabelMsgRecus.Text = "";
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

            // Verification de la presence d'un numero de colis dans la BD
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

                dropAssign.Items.Add(txt_AjoutCamion.Text);
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

            dropAssign.Items.Remove(dropRetirer.SelectedValue);
            dropCamion.Items.Remove(dropRetirer.SelectedValue);
            dropRetirer.Items.Remove(dropRetirer.SelectedValue);
        }
        catch (MySqlException myEx)
        {
        }  

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
        string[] receivedMsg = new string[5];
        int bufMsgLength = 1;

        try {
            Socket sendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPAddress remoteIP = IPAddress.Parse("127.0.0.1");
            const int remotePort = 2160;
            IPEndPoint connectTo = new IPEndPoint(remoteIP, remotePort); ;

            sendSocket.Connect(connectTo);
            byte[] bufSignal = new byte[7];
            bufSignal = System.Text.Encoding.ASCII.GetBytes("update\0");
            sendSocket.Send(bufSignal);

            string myCurrentStr = "";

            while (bufMsgLength > 0) {
                bufMsgLength = sendSocket.Receive(bufMsg);
                if (bufMsgLength > 0)
                    myCurrentStr += System.Text.Encoding.ASCII.GetString(bufMsg, 0, bufMsgLength + 1);
            }

            int i = 0;

            while (myCurrentStr != "") {
                if (myCurrentStr.IndexOf(';') != -1) {
                    receivedMsg[i] = myCurrentStr.Substring(0, myCurrentStr.IndexOf(';'));

                    try {
                        myCurrentStr = myCurrentStr.Substring(myCurrentStr.IndexOf(';') + 2, ((int)myCurrentStr.Length - (myCurrentStr.IndexOf(';') + 2)));
                    } catch {
                        break;
                    }
                } else {
                    myCurrentStr = "";
                }
                i++;
            }

            for (int j = 0; j < i; j++) {
                LabelMsgRecus.Text = receivedMsg[j] + "\n" + LabelMsgRecus.Text;
            }

            sendSocket.Close();

            if (onMsgDiv)
                divMsg.Visible = true;

        } catch (SocketException exp) {
            receivedMsg[0] = "Could not connect to message server" + exp.Message;
        }
    }

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
        MySqlConnection MyConnection = GetConnection();
        MySqlCommand MyCommand = null;
        MySqlDataReader MyReader = null;

        str_Sql = "SELECT * FROM colis";

        MyCommand = new MySqlCommand(str_Sql, MyConnection);
        MyReader = MyCommand.ExecuteReader();

        while (MyReader.Read())
        {
            TableRow row = new TableRow();
            row.BackColor = Color.Black;
            for (int i = 0; i < MyReader.FieldCount; i++)
            {
                TableCell cell = new TableCell();
                Label label = new Label();
                label.Text = MyReader[i].ToString();
                cell.Controls.Add(label);
                row.Cells.Add(cell);
            }
            TBLListeColis.Rows.Clear();
            TBLListeColis.Rows.Add(row);
        }
        MyReader.Close();

        // Close connection if this update comes from a tick
        if (sender != null) {
            MyConnection.Close();
        }

        if (onListeDiv)
            divListe.Visible = true;
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
}
