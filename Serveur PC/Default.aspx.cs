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

public partial class _Default : System.Web.UI.Page 
{
   string str_ConnString = ConfigurationSettings.AppSettings["ConnectionString"];

    protected void Page_Load(object sender, EventArgs e)
    {

    }
    protected void cmd_Ajout_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.DarkOrange;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.Yellow;
    }
    protected void cmd_Carte_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.DarkOrange;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.Yellow;
    }
    protected void cmd_ListeColis_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.DarkOrange;
        cmd_Msg.BackColor = Color.Yellow;
    }
    protected void cmd_Msg_Click(object sender, EventArgs e)
    {
        cmd_Ajout.BackColor = Color.Yellow;
        cmd_Carte.BackColor = Color.Yellow;
        cmd_ListeColis.BackColor = Color.Yellow;
        cmd_Msg.BackColor = Color.DarkOrange;
    }
    protected void cmdValiderAjout_Click(object sender, EventArgs e)
    {
        //AjoutColis();
        //RechercherColis("12345");
        ModifierColis();
    }
    private void AjoutColis()
    {
        try
        {
            string str_Sql = "";
            string str_PlageDebutCli = "";
            string str_PlageFinCli = "";
            string str_PlageDebutDest = "";
            string str_PlageFinDest = "";
            string str_EtatColis = "0";
            MySqlConnection MyConnection = null;
            MySqlCommand MyCommand = null;

            if (rdb_Etat1.Checked == true)
            {
                str_EtatColis = "0";
            }
            else
            {
                str_EtatColis = "1";
            }

            str_PlageDebutCli = txt_PlageClient1.Text + ":00";
            str_PlageFinCli = txt_PlageClient2.Text + ":00";
            str_PlageDebutDest = txt_PlageDest1.Text + ":00";
            str_PlageFinDest = txt_PlageDest2.Text + ":00";

            str_Sql = "INSERT INTO colis (col_noident, col_nomcli, col_adrcli, col_hrdebutcli, col_hrfincli, ";
            str_Sql += "col_remarquecli, col_etat, col_nomdest, col_adrdest, col_hrdebutdest, col_hrfindest, ";
            str_Sql += "col_remarquedest, col_camion) VALUES ('" + txt_Ident.Text + "', '" + txt_NomClient.Text + "', '" + txt_AdresseClient1.Text;
            str_Sql += " " + txt_AdresseClient2.Text + "', '" + str_PlageDebutCli + "', '" + str_PlageFinCli + "', '";
            str_Sql += txt_RemarquesClient1.Text + "', '" + str_EtatColis + "', '" + txt_NomDest.Text + "', '";
            str_Sql += txt_AdresseDest1.Text + " " + txt_AdresseDest2.Text + "', '" + str_PlageDebutDest + "', '" + str_PlageFinDest;
            str_Sql += "', '" + txt_RemarquesDest1.Text + "', '')";

            MyConnection = new MySqlConnection(str_ConnString);
            MyConnection.Open();

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyCommand.ExecuteNonQuery();

        }
        catch (MySqlException myEx)
        {
        }
    }
    private void RechercherColis(string str_ColIdent)
    {
        try
        {
            string str_Sql = "";
            string str_Temp = "";
            MySqlConnection MyConnection = null;
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            str_Sql = "SELECT * FROM colis WHERE col_noident='" + str_ColIdent + "'";

            MyConnection = new MySqlConnection(str_ConnString);
            MyConnection.Open();

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                txt_Ident.Text = MyReader[1].ToString();
                txt_NomClient.Text = MyReader[2].ToString();

                if (MyReader[3].ToString().Length < 100)
                {
                    txt_AdresseClient1.Text = MyReader[3].ToString();
                    txt_AdresseClient2.Text = "";
                }
                else
                {
                    str_Temp = MyReader[3].ToString();
                    txt_AdresseClient1.Text = str_Temp.Substring(0, 99);
                    txt_AdresseClient2.Text = str_Temp.Substring(99, str_Temp.Length - 99);
                }

                txt_PlageClient1.Text = MyReader[4].ToString().Substring(0, 5);
                txt_PlageClient2.Text = MyReader[5].ToString().Substring(0, 5);
                txt_RemarquesClient1.Text = MyReader[6].ToString();

                if (MyReader[7].ToString() == "0")
                {
                    rdb_Etat1.Checked = true;
                    rdb_Etat2.Checked = false;
                }
                else if (MyReader[7].ToString() == "1")
                {
                    rdb_Etat1.Checked = false;
                    rdb_Etat2.Checked = true;
                }

                txt_NomDest.Text = MyReader[8].ToString();

                if (MyReader[9].ToString().Length < 100)
                {
                    txt_AdresseDest1.Text = MyReader[9].ToString();
                    txt_AdresseDest2.Text = "";
                }
                else
                {
                    str_Temp = MyReader[9].ToString();
                    txt_AdresseDest1.Text = str_Temp.Substring(0, 99);
                    txt_AdresseDest2.Text = str_Temp.Substring(99, str_Temp.Length - 99);
                }

                txt_PlageDest1.Text = MyReader[10].ToString().Substring(0, 5);
                txt_PlageDest2.Text = MyReader[11].ToString().Substring(0, 5);
                txt_RemarquesDest1.Text = MyReader[12].ToString();
            }
        }
        catch (MySqlException myEx)
        {
        }
    }
    private void ModifierColis()
    {
        try
        {
            string str_Sql = "";
            string str_PlageDebutCli = "";
            string str_PlageFinCli = "";
            string str_PlageDebutDest = "";
            string str_PlageFinDest = "";
            string str_EtatColis = "0";
            MySqlConnection MyConnection = null;
            MySqlCommand MyCommand = null;

            if (rdb_Etat1.Checked == true)
            {
                str_EtatColis = "0";
            }
            else
            {
                str_EtatColis = "1";
            }

            str_PlageDebutCli = txt_PlageClient1.Text + ":00";
            str_PlageFinCli = txt_PlageClient2.Text + ":00";
            str_PlageDebutDest = txt_PlageDest1.Text + ":00";
            str_PlageFinDest = txt_PlageDest2.Text + ":00";

            str_Sql = "UPDATE colis SET col_noident = '" + txt_Ident.Text + "', col_nomcli = '" + txt_NomClient.Text;
            str_Sql += "', col_adrcli = '" + txt_AdresseClient1.Text + " " + txt_AdresseClient2.Text + "', col_hrdebutcli = '";
            str_Sql += str_PlageDebutCli + "', col_hrfincli = '" + str_PlageFinCli + "', col_remarquecli = '" + txt_RemarquesClient1.Text;
            str_Sql += "', col_etat = '" + str_EtatColis + "', col_nomdest = '" + txt_NomDest.Text + "', col_adrdest = '";
            str_Sql += txt_AdresseDest1.Text + " " + txt_AdresseDest2.Text + "', col_hrdebutdest = '" + str_PlageDebutDest;
            str_Sql += "', col_hrfindest = '" + str_PlageFinDest + "', col_remarquedest ='" + txt_RemarquesDest1.Text;
            str_Sql += "', col_camion = ' ' WHERE col_noident = '" + txt_Ident.Text + "'";

            MyConnection = new MySqlConnection(str_ConnString);
            MyConnection.Open();

            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyCommand.ExecuteNonQuery();
        }
        catch (MySqlException myEx)
        {
        }
    }
}
