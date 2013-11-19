import java.rmi.*;
import java.rmi.server.*;

public class myIRCCallbackImpl extends UnicastRemoteObject
implements myIRCCallbackInterface {

	private static final long serialVersionUID = 1998177431327689435L;

	/**
	 * This object is created client side and is used by the server to communicate with the client
	 */
	public myIRCCallbackImpl() throws RemoteException {
		super( );
	}

	/**
	 * reception of a message from another client through the server
	 */
	public void receiveMsg(String msg, String sender) throws RemoteException {
		System.out.println();
		System.out.println("From " + sender + ": " + msg);	
		System.out.print("#");
	}

	/**
	 * this method is used by the server to inform the client that a user has left the chat
	 */
	public void left(String user) throws RemoteException {
		System.out.println();
		System.out.println(user + " has left");	
		System.out.print("#");
		
	}
	
	/**
	 * this method is used by the server to inform the client that a user has joined the chat
	 */
	public void arrived(String user) throws RemoteException {
		System.out.println();
		System.out.println(user + " has arrived");	
		System.out.print("#");
	}
} 
