<%@ Page Language="C#" AutoEventWireup="true" EnableEventValidation="false" CodeFile="Default.aspx.cs" Inherits="_Default" %>

<%@ Register Assembly="System.Web.Extensions, Version=1.0.61025.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35"
    Namespace="System.Web.UI" TagPrefix="asp" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title>PDA Livraison Express</title>
    
    <script type="text/javascript" language="javascript">

        function changecolor(color) 
        {
            document.bgColor=color;
        }

    </script>

</head>
<body text="white">
    <table border="0" bgcolor="black">
        <tr>
            <td colspan="4" align="CENTER" style="height: 150px; width: 1016px" bgcolor="DarkRed"><asp:Label ID="Label1" runat="server" Font-Size="42"
                    ForeColor="Yellow" Height="150px" Text="PDA Livraison Express" Width="800px"></asp:Label></td></tr>
    </table>   

    <form id="formAjout" runat="server">
        <asp:ScriptManager ID="ScriptManager1" runat="server">
        </asp:ScriptManager>        
        <div id="divMenu" runat="server">
            <table border="0" bgcolor="black">           
                <tr>
                    <td style="width: 70px"><asp:Button ID="cmd_Ajout" BackColor="DarkOrange"
                         Font-Bold="True" ForeColor="DarkRed" runat="server" Text="Ajouter un colis" OnClick="cmd_Ajout_Click" Width="200px" /></td>
                    <td><asp:Button ID="cmd_Carte" BackColor="Yellow"
                         Font-Bold="True" ForeColor="DarkRed" runat="server" Text="Carte de la ville" OnClick="cmd_Carte_Click" Width="200px" /></td>
                    <td><asp:Button ID="cmd_ListeColis" BackColor="Yellow"
                         Font-Bold="True" ForeColor="DarkRed" runat="server" Text="Liste des colis" OnClick="cmd_ListeColis_Click" Width="200px" /></td>
                    <td><asp:Button ID="cmd_Msg" BackColor="Yellow"
                         Font-Bold="True" ForeColor="DarkRed" runat="server" Text="Messages" OnClick="cmd_Msg_Click" Width="200px" /></td>
                    <td><asp:Button ID="cmd_Camion" BackColor="Yellow"
                         Font-Bold="True" ForeColor="DarkRed" runat="server" Text="Gestion des camions" OnClick="cmd_Camion_Click" Width="200px" /></td>
                         
                </tr>
            </table>
        </div>  
    
        <div id="divAjout" runat="server">
            <table border="0" bgcolor="Black" width="1022">        
                <tr><td style="width: 70px"><font size="5">Ajouter un colis</font></td></tr><tr></tr>
                <tr><td style="width: 70px" bgcolor="#AA0"><font size="4">Client</font></td><td>
                    <asp:Label ID="lblError" runat="server" ForeColor="Yellow"
                        Visible="False" Width="205px"></asp:Label></td><td bgcolor="#AA0" style="width: 200px"><font size="4">Destinataire</font></td></tr>
                <tr><td style="width: 70px" bgcolor="#800">No. d'identification</td><td>
                    <asp:TextBox ID="txt_Ident" Width="200px" runat="server"></asp:TextBox></td><td></td><td></td></tr>
                <tr><td style="width: 70px" bgcolor="#800">Nom</td><td>
                    <asp:TextBox ID="txt_NomClient" Width="200px"  runat="server"></asp:TextBox></td><td bgcolor="#800">Nom</td><td>
                        <asp:TextBox ID="txt_NomDest" Width="200px" runat="server"></asp:TextBox></td></tr>        
                <tr><td style="width: 70px" bgcolor="#800">Adresse</td><td>
                    <asp:TextBox ID="txt_AdresseClient1" Width="200px" runat="server"></asp:TextBox></td><td bgcolor="#800">
                        Adresse</td><td>
                        <asp:TextBox ID="txt_AdresseDest1" Width="200px" runat="server"></asp:TextBox></td></tr>
                <tr><td style="width: 70px" bgcolor="#800"></td><td>
                    <asp:TextBox ID="txt_AdresseClient2" Width="200px" runat="server"></asp:TextBox></td><td bgcolor="#800"></td><td>
                        <asp:TextBox ID="txt_AdresseDest2" Width="200px" runat="server"></asp:TextBox></td></tr>
                <tr><td style="width: 70px" bgcolor="#800">Plage horaire &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
                    De:</td><td>
                    <asp:TextBox ID="txt_PlageClient1" Width="200px" runat="server"></asp:TextBox></td><td bgcolor="#800">Plage horaire &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;
                        &nbsp;&nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
                        De:</td><td>
                        <asp:TextBox ID="txt_PlageDest1" Width="200px" runat="server"></asp:TextBox></td></tr>
                <tr><td style="width: 70px" bgcolor="#800">
                    de la cueillette &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;
                    À:</td><td>
                    <asp:TextBox ID="txt_PlageClient2" Width="200px" runat="server"></asp:TextBox></td><td bgcolor="#800">
                        de la cueillette &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
                        &nbsp;&nbsp; &nbsp; &nbsp;&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; À:</td><td>
                        <asp:TextBox ID="txt_PlageDest2" Width="200px" runat="server"></asp:TextBox></td></tr>
                <tr><td style="width: 70px" bgcolor="#800">Remarques</td><td>
                    <asp:TextBox ID="txt_RemarquesClient1" Width="200px" runat="server" Height="50px"></asp:TextBox></td><td bgcolor="#800">Remarques</td><td>
                        <asp:TextBox ID="txt_RemarquesDest1" Width="200px" runat="server" Height="50px"></asp:TextBox></td></tr>
                <tr><td style="width: 70px" bgcolor="#800">État</td>
                    <td>
                        <table>
                            <tr>
                                <td style="height: 18px">Non cueilli</td>
                                <td style="height: 18px"><asp:RadioButton ID="rdb_Etat1" GroupName="Etat" runat="server" /></td>
                            </tr>
                        </table>
                    </td><td></td><td></td>
                </tr>
                <tr><td style="width: 70px" bgcolor="#800"></td>
                    <td>
                        <table>
                            <tr>
                                <td>Cueilli</td>
                                <td><asp:RadioButton ID="rdb_Etat2" GroupName="Etat" runat="server" /></td>
                            </tr>
                        </table>
                    </td><td></td><td></td>
                </tr>
                <tr><td style="width: 70px" bgcolor="#800"></td>
                    <td>
                        <table>
                            <tr>
                                <td>En livraison</td>
                                <td><asp:RadioButton ID="rdb_Etat3" GroupName="Etat" runat="server" /></td>
                            </tr>
                        </table>
                    </td><td></td><td></td>
                </tr>
                <tr><td style="width: 70px" bgcolor="#800"></td>
                    <td>
                        <table>
                            <tr>
                                <td>Livré</td>
                                <td><asp:RadioButton ID="rdb_Etat4" GroupName="Etat" runat="server" /></td>
                            </tr>
                        </table>
                    </td><td></td><td></td>
                </tr>                    
                <tr><td style="width: 70px" bgcolor="#800">Assigner à : </td><td><asp:DropDownList ID="dropAssign" Width="200px" runat="server"></asp:DropDownList></td></tr>
                <tr><td style="width: 70px">
                    <asp:Button ID="cmdValiderAjout" runat="server" BackColor="#FF8000" BorderColor="Black"
                        Font-Bold="True" OnClick="cmdValiderAjout_Click" Text="Ajouter ce colis" Width="200px" /></td>
                        <td><asp:Button ID="cmdValiderModif" runat="server" BackColor="#FF8000" BorderColor="Black"
                        Font-Bold="True" OnClick="cmdValiderModif_Click" Text="Modifier ce colis" Width="200px" /></td>
                        <td><asp:Button ID="cmdValiderRecherche" runat="server" BackColor="#FF8000" BorderColor="Black"
                        Font-Bold="True" OnClick="cmdValiderRecherche_Click" Text="Rechercher ce colis" Width="200px" /></td>
                        <td><asp:Button ID="cmdEffacerChamps" runat="server" BackColor="#FF8000" BorderColor="Black"
                        Font-Bold="True" OnClick="cmdEffacerChamps_Click" Text="Effacer les champs" Width="200px" /></td></tr>                                                           
            </table>
        </div>
        
        <asp:Timer ID="Timer1" Interval="5000" Enabled="true" OnTick="Timer1_Tick" runat="server" />
        <asp:UpdatePanel ID="UpdatePanel1" UpdateMode="Conditional" runat="server">
        <Triggers><asp:AsyncPostBackTrigger ControlID="Timer1" EventName="Tick" /></Triggers>        
            <ContentTemplate>
                <div id="divMsg" runat="server">
                
                    <table border="0" bgcolor="black" width="1022">        
                        <tr><td style="width: 70px"><font size="5">Messages</font></td></tr>
                        <tr><td style="width: 70px"><font size="4">Messages reçus</font></td></tr>
                        <tr><td style="width: 70px; height: 100px" bgcolor="DarkRed"><asp:TextBox ID="LabelMsgRecus" Wrap="true" ReadOnly="true" TextMode="multiline" 
                                                                                          BorderStyle="None" BorderWidth="0" BackColor="DarkRed" Width="1000px" Height="100px"
                                                                                          ForeColor="White" runat="server"></asp:TextBox></td></tr>
                                                                                          
                        <tr><td style="width: 70px"><asp:Button ID="cmdResetMsgRecus" runat="server" BackColor="#FF8000" BorderColor="Black"
                                                            Font-Bold="True" OnClick="cmdResetMsgRecus_Click" Text="Effacer" Width="200px" /></td></tr>
                        
                        <tr><td style="width: 70px"><font size="4">Envoi d'un message</font></td></tr>
                        <tr><td style="width: 70px"><asp:TextBox ID="TextEnvoiMsg" Width="1000px" Height="100px" TextMode="MultiLine" runat="server"></asp:TextBox></td></tr>
                        <tr><td>
                            <table>
                                <tr><td><font size="4">Destinataire : </font></td><td style="width: 70px"><asp:DropDownList ID="dropCamion" width="200px" runat="server"></asp:DropDownList></td></tr>
                            </table></td></tr>
                        <tr><td style="width: 70px"><asp:Button ID="cmdEnvoyerMsg" runat="server" BackColor="#FF8000" BorderColor="Black"
                                                            Font-Bold="True" OnClick="cmdEnvoyerMsg_Click" Text="Envoyer" Width="200px" /></td><td></td></tr>
                    </table>
                </div>
            </ContentTemplate>
            
        </asp:UpdatePanel>
        
        <div id="divCamion" runat="server">
            <table border="0" bgcolor="black" width="1022">
                <tr><td style="width: 70px"><font size="5">Gestion des camions</font></td></tr>
                <tr><td style="width: 70px"><font size="4">Ajouter un camion</font></td></tr>
                <tr><td>
                    <table border="0" bgcolor="black" width="1016">
                        <tr><td><asp:TextBox ID="txt_AjoutCamion" Width="196px" runat="server"></asp:TextBox>
                            <asp:Label ID="lblErrorCam" runat="server" ForeColor="Yellow" Text="  Le camion existe déjà!"
                                Visible="False" Width="205px"></asp:Label></td>
                            <td><asp:Button ID="cmdAjouterCamion" runat="server" BackColor="#FF8000" BorderColor="Black"
                                                    Font-Bold="True" OnClick="cmdAjouterCamion_Click" Text="Ajouter ce camion" Width="200px" /></td>
                            <td><asp:Button ID="cmdDeterminationItineraire" runat="server" BackColor="#FF8000" BorderColor="Black"
                                                    Font-Bold="True" OnClick="cmdDeterminationItineraire_Click" Text="Déterminer les itinéraires" Width="200px" /></td>
                        </tr>
                    </table></td>
                
                </tr>
                <tr><td style="width: 70px"><font size="4">Retirer un camion</font></td></tr>
                <tr><td>
                    <table border="0" bgcolor="black" width="1016">
                        <tr><td><asp:DropDownList ID="dropRetirer" width="200px" runat="server"></asp:DropDownList></td>
                            <td><asp:Button ID="cmdRetirerCamion" runat="server" BackColor="#FF8000" BorderColor="Black"
                                                    Font-Bold="True" OnClick="cmdRetirerCamion_Click" Text="Retirer ce camion" Width="200px" /></td>
                            <td><asp:Button ID="cmdFinJournee" runat="server" BackColor="#FF8000" BorderColor="Black"
                                                    Font-Bold="True" OnClick="cmdFinJournee_Click" Text="Fin de la journée" Width="200px" /></td>
                        </tr>
                    </table></td>
                
                </tr>
                
            </table>    
        </div>
        
        <asp:Timer ID="Timer2" Interval="5000" Enabled="true" OnTick="Timer2_Tick" runat="server"></asp:Timer>
        <asp:UpdatePanel ID="UpdatePanel2" runat="server">
            <ContentTemplate>
                <div id="divListe" visible="true" runat="server">
                    <asp:Table ID="TBLListeColis" runat="server"></asp:Table>
                </div>
            </ContentTemplate>
            <Triggers><asp:AsyncPostBackTrigger ControlID="Timer2" EventName="Tick" /></Triggers>
        </asp:UpdatePanel>
        
    </form>              
</body>
</html>
