/**
 * @version 1.0 2005/08/03
 * @author Philippe Mabilleau ing.
 */

import java.io.*;
import java.net.*;

public class serveur
{
	public final static int DEFAULT_PORT = 2166;
	public final static int BUFFER_SIZE = 80;
	
	static boolean run = true;
	
	static byte[] Buffer;
	
	static int port = DEFAULT_PORT;
	static DatagramSocket leSocket = null;
	
	public static void main(String[] args) 
	{
		Buffer = new byte[BUFFER_SIZE];
		DatagramPacket paquet = new DatagramPacket(Buffer, BUFFER_SIZE);

		if (args.length > 0) 
		{
			try 
			{
				port = Integer.parseInt(args[0]);
			}
			catch (NumberFormatException e) {}
		}

		System.out.println("Listening on port " + port);

		try 
		{
			leSocket = new DatagramSocket(port);
		}
		catch (SocketException se) 
		{
			System.err.println(se);
			System.exit(0);
		}
		catch (IOException e) 
		{
			System.err.println(e);
			System.exit(0);
		}

		while (run) 
		{
			try 
			{
				leSocket.receive(paquet);
				byte[] donnees = new byte[paquet.getLength()];
				System.arraycopy(paquet.getData(), 0, donnees, 0, paquet.getLength());
				String s = new String(Integer.toHexString(donnees[0] & 0xff));
				System.out.println(paquet.getAddress() + " port "
					+ paquet.getPort() + " valeur : " + donnees[0]);
			}
			catch (IOException e) 
			{
				System.err.println(e);
				run = false;
			}
		}
		System.exit(0);
	}
}
