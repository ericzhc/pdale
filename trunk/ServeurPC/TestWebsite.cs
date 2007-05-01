/*
*********************************************************************************************************
* Fichier : TestWebsite.aspx.cs
* Par     : Marc-Étienne Lebeau, Julien Beaumier-Ethier, Richard Labonté, Francis Robichaud, Julien Marchand
* Date    : 2007/04/13
*********************************************************************************************************
*/
using System;
using System.Web;
using NUnit.Framework;

/*
 * Tests the entire website by calling all the available functions.
 * */
[TestNunit]
public class TestWebSite
{
	_Default website = new _Default();

	/*
	 * Adds a new package in the database and then deletes it
	 * */
	[Test]
	public void AjoutColis() 
	{
		website.cmdValiderAjout_Click(null, null);
		// Validates if the package was correctly added
		website.cmdValiderRecherche_Click(null, null);
		// Updates a package
		website.cmdValiderModif_Click(null, null);
		// Remove the package
		website.cmdRetirerColis_Click(null, null);
	}

	/*
	 * Tests the sql connection with the database and then closes it
	 * */
	[Test]
	public void TestConnection() 
	{
		website.GetConnection();
		website.CloseConnection();
	}

	/*
	 * Generates a map
	 * */
	[Test]
	public void TestGenerateMap() 
	{
		website.Timer3_Tick(null, null);
	}

	/*
	 * Sends a message to the message manager
	 * */
	[Test]
	public void TestEnvoiMessage() 
	{
		website.TextEnvoiMsg = new TextBox();
		website.TextEnvoiMsg.Text = "Test d'un message";
		website.cmdEnvoyerMsg_Click(null, null);
	}

	/*
	 * Retrieves the messages on the message manager
	 * */
	[Test]
	public void TestReceiveMessage() 
	{
		website.Timer1_Tick(null, null);
	}

	/*
	 * Retrieves the entire list of packages available in the database
	 * */
	[Test]
	public void TestListeColis()
	{
		website.Timer2_Tick(null, null);
	}

	/*
	 * Regenerates the itinary of a day and affects each package to a truck
	 * */
	[Test]
	public void TestDeterminationItineraire()
	{
		// Affect packages to trucks
		website.cmdDeterminationItineraire_Click(null, null);
		// Undo the modifications
		website.cmdFinJournee_Click(null, null);
	}

	/*
	 * Gets GPS data from an address string
	 * */
	[Test]
	public void TestFindGPS()
	{
		// Retrieve GPS data
		string[] gpsvalues = website.GetGPSFromAdress("1408 RUE DE L'EGLISE;SAINT-LAURENT;QC;H4L2H3;CA");
		// Make sure they are not empty
		NUnit.IsNull(gpsvalues);
	}

	/*
	 * Gets the quantity of a truck and retrieve their name
	 * */
	[Test]
	public void TestGetCamionQty()
	{
		// Retrieve GPS data
		int qty;
		website.GetCamionQuantity(qty);
		// Make sure there is more than one truck
		NUnit.IsNull(qty);

		// Validates if all names are not empty
		string name;
		for (int i=0; i<qty; i++) {
			GetCamionNameFromID(i, name);
			NUnit.IsNull(name);
		}
	}
}
