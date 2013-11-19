import java.rmi.*;
import java.rmi.server.*;

public class myIRCCallbackImpl extends UnicastRemoteObject
implements myIRCCallbackInterface {

	private static final long serialVersionUID = 1998177431327689435L;

	public myIRCCallbackImpl() throws RemoteException {
		super( );
	}

	public void receiveMsg(String msg, String sender) throws RemoteException {
		System.out.println();
		System.out.println("From " + sender + ": " + msg);	
		System.out.print("#");
	}

	public void left(String user) throws RemoteException {
		System.out.println();
		System.out.println(user + " has left");	
		System.out.print("#");
		
	}

	public void arrived(String user) throws RemoteException {
		System.out.println();
		System.out.println(user + " has arrived");	
		System.out.print("#");
	}
} 
