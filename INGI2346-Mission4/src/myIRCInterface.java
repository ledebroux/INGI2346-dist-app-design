import java.util.ArrayList;

public interface myIRCInterface 
extends java.rmi.Remote {

	public void connect(String name, myIRCCallbackInterface callbackObj)
			throws java.rmi.RemoteException;

	public void disconnect(String name)
			throws java.rmi.RemoteException;
	
	public ArrayList<String> who()
			throws java.rmi.RemoteException;

	public void sendMsg(String dest, String sender, String msg)
			throws java.rmi.RemoteException;
}
