<%@ Page Language="C#" AutoEventWireup="true"  CodeFile="Default.aspx.cs" Inherits="_Default" %>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" >
<head runat="server">
    <title>Untitled Page</title>
    
    <script language=javascript>

        function changecolor(color) 
        {
            document.bgColor=color;
        }

    </script>

</head>
<body>
    <form id="form1" runat="server">
        <table border="1">
            <tr>
                <td colspan=4 align=CENTER><asp:Label ID="Label1" runat="server" BackColor="DarkRed" BorderColor="Black" Font-Size="XX-Large"
                        ForeColor="Yellow" Height="75px" Text="PDA LIVRAISON EXPRESS" Width="542px"></asp:Label></td> 
            <tr>
                <td><asp:Button ID="cmd_Ajout" runat="server" BackColor="DarkOrange" BorderColor="Black"
                    Font-Bold="True" ForeColor="DarkRed" Text="Ajouter un colis" OnClick="cmd_Ajout_Click" /></td>
                <td><asp:Button ID="cmd_Carte" runat="server" BackColor="Yellow" BorderColor="Black"
                    Font-Bold="True" ForeColor="DarkRed" Text="Carte de la ville" OnClick="cmd_Carte_Click" /></td>
                <td><asp:Button ID="cmd_ListeColis" runat="server" BackColor="Yellow" BorderColor="Black"
                    Font-Bold="True" ForeColor="DarkRed" Text="Liste des colis" OnClick="cmd_ListeColis_Click" /></td>
                <td><asp:Button ID="cmd_Msg" runat="server" BackColor="Yellow" BorderColor="Black"
                    Font-Bold="True" ForeColor="DarkRed" Text="Messages" OnClick="cmd_Msg_Click" /></td>
            </tr>
            <tr><td>Ajouter un colis</td></tr><tr></tr>
            <tr><td>Client</td><td></td><td>Destinataire</td></tr>
            <tr><td>No. d'indentification</td><td>
                <asp:TextBox ID="txt_Ident" runat="server"></asp:TextBox></td><td></td><td></td></tr>
            <tr><td>Nom</td><td>
                <asp:TextBox ID="txt_NomClient" runat="server"></asp:TextBox></td><td>Nom</td><td>
                    <asp:TextBox ID="txt_NomDest" runat="server"></asp:TextBox></td></tr>        
            <tr><td>Adresse</td><td>
                <asp:TextBox ID="txt_AdresseClient1" runat="server"></asp:TextBox></td><td></td><td>
                    <asp:TextBox ID="txt_AdresseDest1" runat="server"></asp:TextBox></td></tr>
            <tr><td></td><td>
                <asp:TextBox ID="txt_AdresseClient2" runat="server"></asp:TextBox></td><td></td><td>
                    <asp:TextBox ID="txt_AdresseDest2" runat="server"></asp:TextBox></td></tr>
            <tr><td>Plage horaire De:</td><td>
                <asp:TextBox ID="txt_PlageClient1" runat="server"></asp:TextBox></td><td>Plage horaire     De:</td><td>
                    <asp:TextBox ID="txt_PlageDest1" runat="server"></asp:TextBox></td></tr>
            <tr><td>de la cueillette À:</td><td>
                <asp:TextBox ID="txt_PlageClient2" runat="server"></asp:TextBox></td><td>de la cueillette À:</td><td>
                    <asp:TextBox ID="txt_PlageDest2" runat="server"></asp:TextBox></td></tr>
            <tr><td>Remarques</td><td>
                <asp:TextBox ID="txt_RemarquesClient1" runat="server" Height="49px"></asp:TextBox></td><td>Remarques</td><td>
                    <asp:TextBox ID="txt_RemarquesDest1" runat="server" Height="50px"></asp:TextBox></td></tr>
            <tr><td>État</td><td><table><tr><td style="height: 18px">Non cueilli</td><td style="height: 18px">
                <asp:RadioButton ID="rdb_Etat1" runat="server" /></td></tr></table></td><td></td><td></td></tr>
            <tr><td></td><td><table><tr><td>Cueilli</td><td>
                <asp:RadioButton ID="rdb_Etat2" runat="server" /></td></tr></table></td><td></td><td></td></tr>
            <tr><td>
                <asp:Button ID="cmdValiderAjout" runat="server" BackColor="#FF8000" BorderColor="Black"
                    Font-Bold="True" OnClick="cmdValiderAjout_Click" Text="Valider" Width="147px" /></td><td></td><td></td></tr>                                                
        </table>               
    </form>
</body>
</html>
