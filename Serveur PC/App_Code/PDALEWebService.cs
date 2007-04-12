using System;
using System.Data;
using System.Web;
using System.Collections;
using System.Configuration;
using System.Web.Services;
using System.Web.Services.Protocols;
using System.Xml;
using MySql.Data.MySqlClient;

/// <summary>
/// Summary description for PDALEWebService
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
public class PDALEWebService : System.Web.Services.WebService {

    string str_ConnString = ConfigurationSettings.AppSettings["ConnectionString"];

    public PDALEWebService () {

        //Uncomment the following line if using designed components 
        //InitializeComponent(); 
    }

    [WebMethod]
    public string RechercherColis(/*string str_ColIdent*/)
    {
        try
        {
            string str_Sql = "";
            string str_DonneesColis = "";
            MySqlConnection MyConnection = null;
            MySqlCommand MyCommand = null;
            MySqlDataReader MyReader = null;

            //str_Sql = "SELECT * FROM colis WHERE col_noident='" + str_ColIdent + "'";
            str_Sql = "SELECT * FROM colis WHERE col_noident='12345'";

            MyConnection = new MySqlConnection(str_ConnString);
            MyConnection.Open();
            MyCommand = new MySqlCommand(str_Sql, MyConnection);
            MyReader = MyCommand.ExecuteReader();

            while (MyReader.Read())
            {
                str_DonneesColis = MyReader[1].ToString() + ";";
                str_DonneesColis += MyReader[2].ToString() + ";";
                str_DonneesColis += MyReader[3].ToString() + ";";
                str_DonneesColis += MyReader[4].ToString().Substring(0, 5) + ";";
                str_DonneesColis += MyReader[5].ToString().Substring(0, 5) + ";";
                str_DonneesColis += MyReader[6].ToString() + ";";
                str_DonneesColis += MyReader[7].ToString() + ";";
                str_DonneesColis += MyReader[8].ToString() + ";";
                str_DonneesColis += MyReader[9].ToString() + ";";
                str_DonneesColis += MyReader[9].ToString() + ";";
                str_DonneesColis += MyReader[10].ToString().Substring(0, 5) + ";";
                str_DonneesColis += MyReader[11].ToString().Substring(0, 5) + ";";
                str_DonneesColis += MyReader[12].ToString();
            }
            return str_DonneesColis;
        }
        catch (MySqlException myEx)
        {
            return "";
        } 
    }   
}

